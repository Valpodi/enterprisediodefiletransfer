// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <chrono>
#include <filesystem>
#include <iostream>

#include "ClientWrapper.hpp"
#include "clara/clara.hpp"

struct Params
{
  std::string clientAddress;
  std::uint16_t clientPort;
  std::string filename;
  double dataRateMbps;
  std::uint16_t mtuSize;
};

inline Params parseArgs(int argc, char **argv)
{
  bool showHelp = false;
  std::string filename;
  std::string clientAddress;
  std::uint16_t clientPort;
  std::uint16_t mtuSize;
  double dataRateMbps = 0;
  const auto cli = clara::Help(showHelp) |
                   clara::Opt(filename, "filename")["-f"]["--filename"]("name of a file you want to send") |
                   clara::Opt(clientAddress, "client address")["-a"]["--address"]("address send packets to") |
                   clara::Opt(clientPort, "client port")["-c"]["--clientPort"]("port to send packets to") |
                   clara::Opt(mtuSize, "MTU size")["-m"]["--mtu"]("MTU size of the network interface") |
                   clara::Opt(dataRateMbps, "date rate in Megabits per second")["-r"]["--datarate"]("data rate of transfer");

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

  return {clientAddress, clientPort, filename, dataRateMbps, mtuSize};
}

int main(int argc, char **argv)
{
  std::cout << "Starting Enterprise Diode Client application!" << std::endl;
  const auto params = parseArgs(argc, argv);

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
    return 2;
  }
}
