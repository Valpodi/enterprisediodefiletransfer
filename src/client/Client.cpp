// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file

#include "Client.hpp"
#include <chrono>
#include <iostream>
#include <istream>
#include <random>

Client::Client(
  std::shared_ptr<UdpClientInterface> udpClient,
  std::shared_ptr<TimerInterface> timer,
  std::uint16_t maxPayloadSize):
    udpClient(udpClient),
    edTimer(timer),
    maxPayloadSize(maxPayloadSize),
    headerBuffer({}),
    payloadBuffer(maxPayloadSize)
{
}

void Client::send(std::istream& inputStream, const std::string& filename)
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
      return sendFrame(inputStream, filename);
    }
    catch (const std::exception& exception)
    {
      std::cerr << "exception in send frame" << exception.what() << std::endl;
      return false;
    }
  });
}

bool Client::sendFrame(std::istream& inputStream, const std::string& filename)
{
  udpClient->send(generateEDPacket(inputStream, maxPayloadSize, filename));
  return headerBuffer.at(8) != 1;
}

ConstSocketBuffers Client::generateEDPacket(std::istream& inputStream, std::uint32_t payloadSize, const std::string& filename)
{
  if (inputStream.rdbuf()->in_avail())
  {
    const auto payloadLength = inputStream.read((char*)&*(payloadBuffer.begin()), payloadSize).gcount();
    incrementFrameCount();
    return {
      boost::asio::buffer(headerBuffer.data(), EnterpriseDiode::HeaderSizeInBytes),
      boost::asio::buffer(payloadBuffer.data(), (size_t)payloadLength)};
  }
  else
  {
    return addEOFframe(filename);
  }
}

void Client::incrementFrameCount()
{
  ++(*reinterpret_cast<std::uint32_t*>(&headerBuffer.at(4)));
}

void Client::setEOF()
{
  headerBuffer.at(8) = 1;
}

ConstSocketBuffers Client::addEOFframe(const std::string& filename)
{
  incrementFrameCount();
  setEOF();
  payloadBuffer.assign(filename.begin(), filename.end());
  return {
    boost::asio::buffer(headerBuffer.data(), EnterpriseDiode::HeaderSizeInBytes),
    boost::asio::buffer(payloadBuffer.data(), filename.length())};
}

void Client::setSessionID()
{
  const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
  *reinterpret_cast<std::uint32_t*>(&headerBuffer.at(0)) = (std::uint32_t)std::default_random_engine(seed)();
}

boost::posix_time::microseconds calculateTimerPeriod(double dataRateMbps, std::uint32_t mtuSize)
{
  const auto period = std::round((static_cast<double>((mtuSize * 8)) * 1000000) / (dataRateMbps * 1024 * 1024));
  return boost::posix_time::microseconds {static_cast<std::int64_t>(period)};
}