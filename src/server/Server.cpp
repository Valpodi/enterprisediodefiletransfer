// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>
#include "Server.hpp"
#include "StreamInterface.hpp"

Server::Server(
  std::unique_ptr<UdpServerInterface> udpServerInterface,
  PacketQueueSettings queueSettings,
  std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
  std::function<std::time_t()> getTime,
  std::uint32_t timeoutPeriod,
  DiodeType diodeType) :
    udpServerInterface(std::move(udpServerInterface)),
    sessionManager(
      queueSettings,
      std::move(streamCreator),
      std::move(getTime),
      timeoutPeriod,
      diodeType)
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
