// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "UdpServer.hpp"
#include <diodeheader/EnterpriseDiodeHeader.hpp>
#include <iostream>

UdpServer::UdpServer(
  std::uint16_t port,
  boost::asio::io_service& io_service,
  std::uint32_t udpFrameSize,
  std::uint32_t udpSocketBufferSizeInBytes) :
  udpFrameSize(udpFrameSize),
  io_context(io_service),
  udpSocket(io_service, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), port))
{
  if (udpFrameSize < EnterpriseDiode::HeaderSizeInBytes)
  {
    throw std::runtime_error("UDP Frame size MUST be greater than 112 bytes (was: " + std::to_string(udpFrameSize) + ")");
  }
  udpSocket.set_option(boost::asio::socket_base::receive_buffer_size(static_cast<int>(udpSocketBufferSizeInBytes)));
  triggerWaitAndReadNextUdpPacket();
}

UdpServer::~UdpServer()
{
  io_context.stop();
}

void UdpServer::triggerWaitAndReadNextUdpPacket()
{
  frame = std::vector<std::uint8_t>(udpFrameSize - EnterpriseDiode::HeaderSizeInBytes);
  header = std::vector<std::uint8_t>(EnterpriseDiode::HeaderSizeInBytes);
  std::array<boost::asio::mutable_buffer, 2> bufs = { boost::asio::buffer(header), boost::asio::buffer(frame) };

  udpSocket.async_receive_from(
    bufs,
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
  if (callback && udpPacketLength - EnterpriseDiode::HeaderSizeInBytes > 0)
  {
    frame.resize(udpPacketLength - EnterpriseDiode::HeaderSizeInBytes);
    callback(std::move(header), std::move(frame));
  }
  else
  {
    std::cerr << "insufficient data in payload" << "\n";
  }
}
