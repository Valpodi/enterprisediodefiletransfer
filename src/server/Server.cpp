// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#include <iostream>
#include "Server.hpp"
#include "StreamInterface.hpp"

Server::Server(
  std::unique_ptr<UdpServerInterface> udpServerInterface,
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
  std::function<std::time_t()> getTime,
  std::uint32_t timeoutPeriod) :
    udpServerInterface(std::move(udpServerInterface)),
    sessionManager(maxBufferSize, maxQueueLength, std::move(streamCreator), std::move(getTime), timeoutPeriod)
{
  this->udpServerInterface->setCallback([this](std::istream& payload) { receivePacket(payload); });
}

void Server::receivePacket(std::istream& inputStream)
{
  try
  {
    sessionManager.writeToStream(parsePacket(inputStream));
  }
  catch (const std::runtime_error& exception)
  {
    std::cerr << std::string("Caught exception: ") + exception.what() << std::endl;
  }
}
