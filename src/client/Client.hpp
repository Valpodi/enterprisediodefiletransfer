// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_EDCLIENT_HPP
#define ENTERPRISEDIODE_EDCLIENT_HPP

#include <istream>
#include <boost/asio/time_traits.hpp>
#include <boost/asio/buffer.hpp>
#include "TimerInterface.hpp"
#include "UdpClientInterface.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

class Client
{
public:
  Client(std::shared_ptr<UdpClientInterface> udpClient,
    std::shared_ptr<TimerInterface> timer,
    std::uint16_t maxPayloadSize);

  void send(std::istream& inputStream, const std::string& filename);

private:

  bool sendFrame(std::istream& inputStream, const std::string& filename);
  ConstSocketBuffers generateEDPacket(std::istream& inputStream, std::uint32_t maxPayloadSize, const std::string& filename);
  void incrementFrameCount();
  void setEOF();
  void setSessionID();
  ConstSocketBuffers addEOFframe(const std::string& filename);

  std::shared_ptr<UdpClientInterface> udpClient;
  std::shared_ptr<TimerInterface> edTimer;
  std::uint32_t maxPayloadSize;
  std::array<char, EnterpriseDiode::HeaderSizeInBytes> headerBuffer;
  std::vector<char> payloadBuffer;
};

boost::posix_time::microseconds calculateTimerPeriod(double dataRateMbps, std::uint32_t packetSizeBytes);

#endif //ENTERPRISEDIODE_EDCLIENT_HPP
