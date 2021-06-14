// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef TESTHELPERS_HPP
#define TESTHELPERS_HPP

#include <sstream>
#include <boost/asio/io_service.hpp>
#include "client/UdpClientInterface.hpp"
#include "UdpServerInterface.hpp"
#include "client/Client.hpp"
#include <Server.hpp>
#include "BytesBuffer.hpp"


BytesBuffer createTestPacketStream(std::uint8_t sessionID, std::uint8_t frameCount, std::uint8_t eofFlag);

Server createEdServer(std::unique_ptr<UdpServerInterface> udpServer,
  std::uint32_t maxBufferSize,
  std::uint32_t maxQueueLength,
  std::uint32_t& capturedSessionId,
  std::stringstream& outputStream,
  DiodeType diodeType);

class UdpServerFake : public UdpServerInterface
{
public:
  explicit UdpServerFake(std::uint16_t, boost::asio::io_service&, std::uint32_t, std::uint32_t)
  {
  }
};

class UdpClientSpy : public UdpClientInterface
{
public:
  std::vector<BytesBuffer> buffersSent;
  BytesBuffer latestPacket;

  void send(ConstSocketBuffers inputBuffers) override
  {
    latestPacket = BytesBuffer(boost::asio::buffer_size(inputBuffers));
    boost::asio::buffer_copy(boost::asio::buffer(latestPacket), inputBuffers);
    buffersSent.push_back(latestPacket);
  }
};

class ManualTimer : public TimerInterface
{
public:
  void runTimer(std::function<bool()> callback) override
  {
    tickCallback = callback;
    callback();
  }
};

class FreeRunningTimer : public TimerInterface
{
public:
  void runTimer(std::function<bool()> callback) override
  {
    tickCallback = callback;
    while (callback()) {}
  }
};

#endif //TESTHELPERS_HPP