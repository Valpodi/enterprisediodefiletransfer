// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODE_UDPSERVER_HPP
#define ENTERPRISEDIODE_UDPSERVER_HPP

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_service.hpp>
#include "UdpServerInterface.hpp"
#include <boost/asio/streambuf.hpp>

class UdpServer : public UdpServerInterface
{
public:
  UdpServer(
    std::uint16_t port,
    boost::asio::io_service& io_service,
    std::uint32_t udpFrameSize,
    std::uint32_t udpSocketBufferSizeInBytes = 268435456);

  ~UdpServer() override;

private:
  void triggerWaitAndReadNextUdpPacket();
  void checkPacketLengthAndExecuteCallback(size_t udpPacketLength);
  const std::uint32_t udpFrameSize;
  boost::asio::io_service& io_context;
  boost::asio::ip::udp::socket udpSocket;
  boost::asio::ip::udp::endpoint senderEndpoint;
  boost::asio::streambuf receiveStreamBuffer;
  std::istream outputPacketStream;
};

#endif //ENTERPRISEDIODE_UDPSERVER_HPP
