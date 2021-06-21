// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <utility>
#include "spdlog/spdlog.h"
#include "ClientWrapper.hpp"
#include "Timer.hpp"

ClientWrapper::ClientWrapper(
  const std::string& targetAddress,
  std::uint16_t targetPort,
  std::uint16_t mtuSize,
  double dataRateMbps,
  std::string filename,
  const std::string& logLevel) :
    edClient(std::make_shared<UdpClient>(targetAddress, targetPort),
             std::make_shared<Timer>(calculateTimerPeriod(dataRateMbps, mtuSize)),
             calculatePayloadSize(mtuSize),
             std::move(filename))
{
  spdlog::set_level(spdlog::level::from_str(logLevel));
}

void ClientWrapper::sendData(const std::string& filename)
{
  std::ifstream inputStream(filename, std::ios::binary);
  try
  {
    edClient.send(inputStream);
  }
  catch(const std::exception& exc)
  {
    spdlog::error(std::string("Exception Sending Data: ") + exc.what());
    throw std::runtime_error("sendData failed");
  }
  spdlog::info("Send complete");
}

std::uint16_t calculatePayloadSize(std::uint16_t mtuSize)
{
  return std::uint16_t(
    (short) EnterpriseDiode::calculateMaxBufferSize(mtuSize) - (short) EnterpriseDiode::HeaderSizeInBytes);
}
