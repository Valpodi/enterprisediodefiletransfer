// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "UdpServer.hpp"

UdpServer::UdpServer(
  std::uint16_t port,
  boost::asio::io_service& io_service,
  std::uint32_t udpFrameSize,
  std::uint32_t udpSocketBufferSizeInBytes) :
  udpFrameSize(udpFrameSize),
  io_context(io_service),
  udpSocket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port)),
  outputPacketStream(&receiveStreamBuffer)
{
  udpSocket.set_option(boost::asio::socket_base::receive_buffer_size(static_cast<int>(udpSocketBufferSizeInBytes)));
  triggerWaitAndReadNextUdpPacket();
}

UdpServer::~UdpServer()
{
  io_context.stop();
}

void UdpServer::triggerWaitAndReadNextUdpPacket()
{
  udpSocket.async_receive_from(
    receiveStreamBuffer.prepare(udpFrameSize),
    senderEndpoint,
    [this](boost::system::error_code errorCode, std::size_t udpPacketLength) {
      if (!errorCode)
      {
        checkPacketLengthAndExecuteCallback(udpPacketLength);
        triggerWaitAndReadNextUdpPacket();
      }
    }
  );
}

void UdpServer::checkPacketLengthAndExecuteCallback(size_t udpPacketLength)
{
  if (callback && udpPacketLength > 0)
  {
    receiveStreamBuffer.commit(udpPacketLength);
    callback(outputPacketStream);
    receiveStreamBuffer.consume(udpPacketLength);
  }
}
