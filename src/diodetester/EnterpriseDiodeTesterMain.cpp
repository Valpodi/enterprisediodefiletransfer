// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>
#include <csignal>
#include <chrono>
#include <future>

#include "clara/clara.hpp"
#include "spdlog/spdlog.h"

#include "FileStream.hpp"
#include "UdpServer.hpp"
#include "Server.hpp"
#include "client/ClientWrapper.hpp"
#include "SessionManager.hpp"

struct Params
{
  std::string clientAddress;
  std::uint16_t clientPort;
  std::uint16_t serverPort;
  std::string filename;
  double dataRateMbps;
  std::uint16_t mtuSize;
  std::uint16_t maxQueueLength;
  bool dropPackets;
  DiodeType diodeType;
  std::string logLevel;
};

inline Params parseArgs(int argc, char **argv)
{
  bool showHelp = false;
  std::string clientAddress;
  std::uint16_t clientPort;
  std::uint16_t serverPort;
  std::string filename;
  double dataRateMbps = 0;
  std::uint16_t mtuSize = 1500;
  std::uint16_t maxQueueLength = 1024;
  bool dropPackets = false;
  bool importDiode = false;
  std::string logLevel = "info";
  const auto cli = clara::Help(showHelp) |
                   clara::Opt(clientAddress, "client address")["-a"]["--address"]("address send packets to").required() |
                   clara::Opt(clientPort, "client port")["-c"]["--clientPort"]("port to send packets to").required() |
                   clara::Opt(serverPort, "server port")["-s"]["--serverPort"]("port to listen for packets on").required() |
                   clara::Opt(filename, "filename")["-f"]["--filename"]("name of a file you want to send").required() |
                   clara::Opt(dataRateMbps, "date rate in Megabits per second")["-r"]["--datarate"](
                     "data rate of transfer. default as fast as possible") |
                   clara::Opt(mtuSize, "MTU size")["-m"]["--mtu"]("MTU size of the network interface") |
                   clara::Opt(maxQueueLength, "Queue Length")["-q"]["--queueLength"](
                     "Max length of queue for reordering packets") |
                   clara::Opt(dropPackets)["-d"]["--dropPackets"](
                     "Server will drop all received packets and only show missing packets") |
                   clara::Opt(importDiode)["-i"]["--importDiode"](
                     "Set flag if using an import diode so that the server rewraps data before writing to file.") |
                   clara::Opt(logLevel, "Log level")["-l"]["--logLevel"]("Logging level for program output - default info");

  const auto result = cli.parse(clara::Args(argc, argv));
  if (!result)
  {
    spdlog::error(std::string("Unable to parse command line args: ") + result.errorMessage());
    exit(1);
  }

  if (showHelp)
  {
    std::stringstream helpText;
    helpText << cli;
    spdlog::info(helpText.str());
    exit(1);
  }

  auto diodeType = DiodeType::basic;
  if (importDiode)
  {
    diodeType = DiodeType::import;
  }

  spdlog::set_level(spdlog::level::from_str(logLevel));
  return {clientAddress, clientPort, serverPort, filename, dataRateMbps, mtuSize, maxQueueLength, dropPackets,
          diodeType, logLevel};
}

namespace EDTesterApplication
{
  void signalHandler(int signum);
  boost::asio::io_service io_context;

  void signalHandler(int)
  {
    EDTesterApplication::io_context.stop();
    spdlog::info("SIGINT Received, stopping Tester.");
  }
}

int main(int argc, char **argv)
{
  const auto params = parseArgs(argc, argv);
  spdlog::info("Starting Enterprise Diode Tester application.");
  signal(SIGINT, EDTesterApplication::signalHandler);

  auto maxBufferSize = EnterpriseDiode::calculateMaxBufferSize(params.mtuSize);

  Server edServer(
    std::make_unique<UdpServer>(
      params.serverPort,
      EDTesterApplication::io_context,
      maxBufferSize,
      EnterpriseDiode::UDPSocketSizeInBytes),
    maxBufferSize,
    params.maxQueueLength,
    [](std::uint32_t sessionId)
    { return std::make_unique<FileStream>(sessionId); },
    []()
    { return std::time(nullptr); }, 15, params.diodeType);

  auto handleToSendingProcess = std::async(
    std::launch::async, []() {
      EDTesterApplication::io_context.run();
    });

  while (EDTesterApplication::io_context.stopped()) { usleep(100); }

  try
  {
    ClientWrapper(
      params.clientAddress,
      params.clientPort,
      params.mtuSize,
      params.dataRateMbps,
      params.filename,
      params.logLevel,
      0
    ).sendData(params.filename);
  }
  catch (const std::exception& exception)
  {
    spdlog::error(std::string("Caught exception: ") + exception.what());
    EDTesterApplication::io_context.stop();
    return 2;
  }
}
