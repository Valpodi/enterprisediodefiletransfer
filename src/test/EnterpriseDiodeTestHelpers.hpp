// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_TESTHELPERS_HPP
#define ENTERPRISEDIODE_TESTHELPERS_HPP

#include <sstream>
#include <boost/asio/io_service.hpp>
#include "client/UdpClientInterface.hpp"
#include "UdpServerInterface.hpp"
#include "client/Client.hpp"
#include <Server.hpp>
#include "BytesBuffer.hpp"

std::stringstream createTestPacketStream(std::vector<char> data, char sessionID, char frameCount, bool eofFlag);

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

#endif //ENTERPRISEDIODE_TESTHELPERS_HPP