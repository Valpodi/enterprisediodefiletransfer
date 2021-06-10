// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>
#include <chrono>
#include <csignal>

#include "clara/clara.hpp"
#include "Server.hpp"
#include "UdpServer.hpp"
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
  std::uint16_t serverPort;
  std::uint16_t mtuSize;
  std::uint16_t maxQueueLength = 1024;
  bool dropPackets = false;
  bool importDiode = false;
  DiodeType diodeType;
  const auto cli = clara::Help(showHelp) |
                   clara::Opt(serverPort, "server port")["-s"]["--serverPort"]("port to listen for packets on") |
                   clara::Opt(mtuSize, "MTU size")["-m"]["--mtu"]("MTU size of the network interface") |
                   clara::Opt(maxQueueLength, "Queue Length")["-q"]["--queueLength"](
                     "Max length of queue for reordering packets") |
                   clara::Opt(dropPackets)["-d"]["--dropPackets"](
                     "Diagnostic tool: Server will not write packets to disk if this flag set (will only count missing frames), else will write them to a file as normal") |
                   clara::Opt(importDiode, "import diode")["-i"]["--importDiode"]("import diode flag for rewrapper");

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

  if (importDiode)
  {
    diodeType = DiodeType::import;
  }
  else
  {
    diodeType = DiodeType::basic;
  }

  return {serverPort, mtuSize, maxQueueLength, dropPackets, diodeType};
}

namespace ServerApplication
{
  void signalHandler(int signum);
  boost::asio::io_service io_context;

  void signalHandler(int)
  {
    ServerApplication::io_context.stop();
    std::cout << "SIGINT Received, stopping Server" << std::endl;
  }
}

int main(int argc, char **argv)
{
  std::cout << "Starting Enterprise Diode Server application!" << std::endl;
  signal(SIGINT, ServerApplication::signalHandler);

  const auto params = parseArgs(argc, argv);

  const auto maxBufferSize = EnterpriseDiode::calculateMaxBufferSize(params.mtuSize);

  try
  {
    Server edServer(
      std::make_unique<UdpServer>(
        params.serverPort,
        ServerApplication::io_context,
        maxBufferSize,
        EnterpriseDiode::UDPSocketSizeInBytes),
      {maxBufferSize, params.maxQueueLength, params.dropPackets},
      selectWriteStreamFunction(params.dropPackets),
      []() { return std::time(nullptr); }, 15, params.diodeType);

    ServerApplication::io_context.run();
  }
  catch (const std::runtime_error& exception)
  {
    std::cerr << std::string("Caught exception: ") + exception.what() << std::endl;
    throw;
  }
}
