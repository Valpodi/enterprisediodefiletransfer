// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file

#include "Client.hpp"
#include <chrono>
#include <iostream>
#include <istream>
#include <random>
#include <filesystem>
#include <boost/algorithm/string.hpp>
#include <regex>

Client::Client(
  std::shared_ptr<UdpClientInterface> udpClient,
  std::shared_ptr<TimerInterface> timer,
  std::uint16_t maxPayloadSize,
  std::string filename):
    udpClient(udpClient),
    edTimer(timer),
    maxPayloadSize(maxPayloadSize),
    headerBuffer({}),
    payloadBuffer(maxPayloadSize),
    filename(std::move(filename))
{
}

void Client::send(std::istream& inputStream)
{
  inputStream.exceptions(std::istream::badbit);
  if (!inputStream)
  {
    throw std::runtime_error("file stream not found");
  }
  parseFilename();
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

void Client::parseFilename()
{
    std::vector<std::string> segments;
    boost::split(segments, filename, boost::is_any_of("/"));
    filename = segments.back();
    std::regex filter("[a-zA-Z0-9\\.\\-_]+");
    if (!std::regex_match(filename, filter))
    {
      throw std::runtime_error("Invalid filename provided. Please rename.");
    }
}

bool Client::sendFrame(std::istream& inputStream)
{
  udpClient->send(generateEDPacket(inputStream, maxPayloadSize));
  return headerBuffer.at(8) != 1;
}

ConstSocketBuffers Client::generateEDPacket(std::istream& inputStream, std::uint32_t payloadSize)
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
    return addEOFframe();
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

ConstSocketBuffers Client::addEOFframe()
{
  incrementFrameCount();
  setEOF();
  std::string filenameAsSisl = "{name: !str \"" + filename + "\"}";
  return {
    boost::asio::buffer(headerBuffer.data(), EnterpriseDiode::HeaderSizeInBytes),
    boost::asio::buffer(filenameAsSisl.data(), filenameAsSisl.length())};
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