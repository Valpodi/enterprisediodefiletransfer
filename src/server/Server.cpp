// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <iostream>
#include "Server.hpp"
#include "StreamInterface.hpp"

Server::Server(
  std::unique_ptr<UdpServerInterface> udpServerInterface,
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
  std::function<std::time_t()> getTime,
  std::uint32_t timeoutPeriod,
  DiodeType diodeType) :
    udpServerInterface(std::move(udpServerInterface)),
    sessionManager(maxBufferSize, maxQueueLength, std::move(streamCreator), std::move(getTime), timeoutPeriod, diodeType)
{
  this->udpServerInterface->setCallback([this](std::vector<std::uint8_t>&& header, std::vector<std::uint8_t>&& payload) { receivePacket(std::move(header), std::move(payload)); });
}

void Server::receivePacket(std::vector<std::uint8_t>&& header, std::vector<std::uint8_t>&& payload)
{
  try
  {
    sessionManager.writeToStream(parsePacket(std::move(header), std::move(payload)));
  }
  catch (const std::runtime_error& exception)
  {
    std::cerr << std::string("Caught exception: ") + exception.what() << std::endl;
  }
}
