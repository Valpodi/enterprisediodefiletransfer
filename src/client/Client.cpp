// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#include <istream>
#include <random>
#include <chrono>
#include <iostream>
#include "Client.hpp"

Client::Client(
  std::shared_ptr<UdpClientInterface> udpClient,
  std::shared_ptr<TimerInterface> timer,
  std::uint16_t maxPayloadSize) :
    udpClient(udpClient),
    edTimer(timer),
    maxPayloadSize(maxPayloadSize),
    headerBuffer({}),
    payloadBuffer(maxPayloadSize)
{
}

void Client::send(std::istream& inputStream)
{
  inputStream.exceptions(std::istream::badbit);
  if (!inputStream)
  {
    throw std::runtime_error("file stream not found");
  }
  setSessionID();
  edTimer->runTimer([&]() {
    try
    {
      return sendFrame(inputStream);
    }
    catch (const std::exception& exception)
    {
      std::cerr << "exception in send frame" << exception.what() << std::endl;
      return false;
    }
  });
}

bool Client::sendFrame(std::istream& inputStream)
{
  udpClient->send(generateEDPacket(inputStream, maxPayloadSize));
  return inputStream.rdbuf()->in_avail();
}

ConstSocketBuffers Client::generateEDPacket(std::istream& inputStream, std::uint32_t payloadSize)
{
  const auto payloadLength = inputStream.read((char*) &*(payloadBuffer.begin()), payloadSize).gcount();
  incrementFrameCount();
  if (!inputStream.rdbuf()->in_avail())
  {
    setEOF();
  }
  return {boost::asio::buffer(headerBuffer.data(), EnterpriseDiode::HeaderSizeInBytes),
          boost::asio::buffer(payloadBuffer.data(), (size_t) payloadLength)};
}

void Client::incrementFrameCount()
{
  ++(*reinterpret_cast<std::uint32_t*>(&headerBuffer.at(4)));
}

void Client::setEOF()
{
  headerBuffer.at(8) = 1;
}

void Client::setSessionID()
{
  const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  *reinterpret_cast<std::uint32_t*>(&headerBuffer.at(0)) = (std::uint32_t) std::default_random_engine(seed)();
}

boost::posix_time::microseconds calculateTimerPeriod(double dataRateMbps, std::uint32_t mtuSize)
{
  const auto period = std::round((static_cast<double>((mtuSize * 8)) * 1000000) / (dataRateMbps * 1024 * 1024));
  return boost::posix_time::microseconds{static_cast<std::int64_t>(period)};
}