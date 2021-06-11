// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef SERVER_HPP
#define SERVER_HPP

#include <memory>
#include <boost/asio/io_service.hpp>
#include "UdpServerInterface.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "SessionManager.hpp"
#include "Packet.hpp"

class StreamInterface;

class Server
{
public:
  Server(std::unique_ptr<UdpServerInterface> udpServerInterface,
    std::uint32_t maxBufferSize,
    std::uint32_t maxQueueLength,
    std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
    std::function<std::time_t()> getTime,
    std::uint32_t timeoutPeriod,
   DiodeType diodeType);

  void receivePacket(std::istream& inputStream);

private:
  std::unique_ptr<UdpServerInterface> udpServerInterface;
  SessionManager sessionManager;
};

#endif //ENTERPRISEDIODE_EDSERVER_HPP
