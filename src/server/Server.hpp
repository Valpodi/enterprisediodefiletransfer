// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_EDSERVER_HPP
#define ENTERPRISEDIODE_EDSERVER_HPP

#include <memory>
#include <boost/asio/io_service.hpp>
#include "UdpServerInterface.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "SessionManager.hpp"
#include "Packet.hpp"
#include "FileStream.hpp"
#include "DropStream.hpp"

class StreamInterface;

class Server
{
public:
  Server(
    std::unique_ptr<UdpServerInterface> udpServerInterface,
    PacketQueueSettings queueSettings,
    std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
    std::function<std::time_t()> getTime,
    std::uint32_t timeoutPeriod,
    DiodeType diodeType);

  void receivePacket(std::istream& inputStream);

private:
  std::unique_ptr<UdpServerInterface> udpServerInterface;
  SessionManager sessionManager;
};

inline std::function<std::unique_ptr<StreamInterface>(uint32_t)> selectWriteStreamFunction(bool dropPackets)
{
  if (dropPackets)
  {
    return [](uint32_t sessionId) { return std::make_unique<DropStream>(sessionId); };
  }
  return [](uint32_t sessionId) { return std::make_unique<FileStream>(sessionId); };
}

#endif //ENTERPRISEDIODE_EDSERVER_HPP
