// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <chrono>
#include <csignal>

#include "clara/clara.hpp"
#include "spdlog/spdlog.h"

#include "Server.hpp"
#include "UdpServer.hpp"
#include "FileStream.hpp"
#include "DropStream.hpp"

struct Params
{
  std::uint16_t serverPort;
  std::uint16_t mtuSize;
  std::uint16_t maxQueueLength;
  bool dropPackets;
  DiodeType diodeType;
};

inline Params parseArgs(int argc, char **argv)
{
  bool showHelp = false;
  std::uint16_t serverPort = 45000;
  std::uint16_t mtuSize = 1500;
  std::uint16_t maxQueueLength = 1024;
  bool dropPackets = false;
  bool importDiode = false;
  std::string logLevel = "info";
  const auto cli = clara::Help(showHelp) |
                   clara::Opt(serverPort, "server port")["-s"]["--serverPort"]("port to listen for packets on - default 45000") |
                   clara::Opt(mtuSize, "MTU size")["-m"]["--mtu"]("MTU size of the network interface - default 1500") |
                   clara::Opt(maxQueueLength, "Queue Length")["-q"]["--queueLength"](
                     "Max length of queue for reordering packets - default 1024 packets") |
                   clara::Opt(dropPackets)["-d"]["--dropPackets"](
                     "Diagnostic tool: Server will not write packets to disk if this flag set (will only count missing frames), else will write them to a file as normal") |
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
  return {serverPort, mtuSize, maxQueueLength, dropPackets, diodeType};
}

namespace ServerApplication
{
  void signalHandler(int signum);
  boost::asio::io_service io_context;

  void signalHandler(int)
  {
    ServerApplication::io_context.stop();
    spdlog::info("SIGINT Received, stopping Server.");
  }
}

inline std::function<std::unique_ptr<StreamInterface>(uint32_t)> selectWriteStreamFunction(bool dropPackets)
{
  if (dropPackets)
  {
    return [](uint32_t sessionId) { return std::make_unique<DropStream>(sessionId); };
  }
  return [](uint32_t sessionId) { return std::make_unique<FileStream>(sessionId); };
}


int main(int argc, char **argv)
{
  const auto params = parseArgs(argc, argv);
  spdlog::info("Starting Enterprise Diode Server application.");
  signal(SIGINT, ServerApplication::signalHandler);

  const auto maxBufferSize = EnterpriseDiode::calculateMaxBufferSize(params.mtuSize);

  try
  {
    Server edServer(
      std::make_unique<UdpServer>(
        params.serverPort,
        ServerApplication::io_context,
        maxBufferSize,
        EnterpriseDiode::UDPSocketSizeInBytes),
      maxBufferSize,
      params.maxQueueLength,
      selectWriteStreamFunction(params.dropPackets),
      []() { return std::time(nullptr); }, 15, params.diodeType);

    ServerApplication::io_context.run();
  }
  catch (const std::runtime_error& exception)
  {
    spdlog::error(std::string("Caught exception: ") + exception.what());
    throw;
  }
}
