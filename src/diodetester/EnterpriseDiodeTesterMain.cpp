// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>
#include <csignal>
#include <chrono>
#include <future>
#include <FileStream.hpp>

#include "clara/clara.hpp"
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
};

inline Params parseArgs(int argc, char **argv)
{
  bool showHelp = false;
  std::string clientAddress;
  std::uint16_t clientPort;
  std::uint16_t serverPort;
  std::string filename;
  double dataRateMbps;
  std::uint16_t mtuSize;
  std::uint16_t maxQueueLength = 1024;
  bool dropPackets = false;
  bool importDiode = false;
  const auto cli = clara::Help(showHelp) |
                   clara::Opt(clientAddress, "client address")["-a"]["--address"]("address send packets to") |
                   clara::Opt(clientPort, "client port")["-c"]["--clientPort"]("port to send packets to") |
                   clara::Opt(serverPort, "server port")["-s"]["--serverPort"]("port to listen for packets on") |
                   clara::Opt(filename, "filename")["-f"]["--filename"]("name of a file you want to send") |
                   clara::Opt(dataRateMbps, "date rate in Megabits per second")["-r"]["--datarate"](
                     "data rate of transfer") |
                   clara::Opt(mtuSize, "MTU size")["-m"]["--mtu"]("MTU size of the network interface") |
                   clara::Opt(maxQueueLength, "Queue Length")["-q"]["--queueLength"](
                     "Max length of queue for reordering packets") |
                   clara::Opt(dropPackets)["-d"]["--dropPackets"](
                     "Server will write packets to disk if this flag is false, else will drop them and only count missing packets") |
                   clara::Opt(importDiode, "import diode")["-i"]["--importDiode"](
                     "Set flag if using an import diode so that the server rewraps data before writing to file.");

  const auto result = cli.parse(clara::Args(argc, argv));
  if (!result)
  {
    std::cerr << "Unable to parse command line args: " << result.errorMessage() << std::endl;
    exit(1);
  }

  if (showHelp)
  {
    std::cout << cli << std::endl;
    exit(1);
  }

  auto diodeType = DiodeType::basic;
  if (importDiode)
  {
    diodeType = DiodeType::import;
  }

  return {clientAddress, clientPort, serverPort, filename, dataRateMbps, mtuSize, maxQueueLength, dropPackets,
          diodeType};
}

namespace EDTesterApplication
{
  void signalHandler(int signum);
  boost::asio::io_service io_context;

  void signalHandler(int)
  {
    EDTesterApplication::io_context.stop();
    std::cout << "SIGINT Received, stopping Server" << std::endl;
  }
}

int main(int argc, char **argv)
{
  std::cout << "Starting Enterprise Diode Tester application!" << std::endl;

  signal(SIGINT, EDTesterApplication::signalHandler);
  const auto params = parseArgs(argc, argv);

  auto maxBufferSize = EnterpriseDiode::calculateMaxBufferSize(params.mtuSize);

  Server edServer(
    std::make_unique<UdpServer>(
      params.serverPort, EDTesterApplication::io_context, maxBufferSize, EnterpriseDiode::UDPSocketSizeInBytes),
    maxBufferSize, params.maxQueueLength, false,
    [](std::uint32_t sessionId) { return std::make_unique<FileStream>(sessionId); },
    []() { return std::time(nullptr); }, 15, params.diodeType);

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
      params.filename
    ).sendData(params.filename);
  }
  catch (const std::exception& exception)
  {
    std::cerr << std::string("Caught exception: ") + exception.what() << std::endl;
    EDTesterApplication::io_context.stop();
    return 2;
  }
}
