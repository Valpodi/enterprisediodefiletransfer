// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "ClientWrapper.hpp"

#include <utility>
#include "Timer.hpp"

ClientWrapper::ClientWrapper(
  const std::string& targetAddress,
  std::uint16_t targetPort,
  std::uint16_t mtuSize,
  double dataRateMbps,
  std::string filename) :
    edClient(std::make_shared<UdpClient>(targetAddress, targetPort),
             std::make_shared<Timer>(calculateTimerPeriod(dataRateMbps, mtuSize)),
             calculatePayloadSize(mtuSize),
             std::move(filename))
{
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
    std::cerr << "exception here " << exc.what() << std::endl;
    throw std::runtime_error("sendData failed");
  }
  std::cout << "Send complete" << std::endl;
}

std::uint16_t calculatePayloadSize(std::uint16_t mtuSize)
{
  return std::uint16_t(
    (short) EnterpriseDiode::calculateMaxBufferSize(mtuSize) - (short) EnterpriseDiode::HeaderSizeInBytes);
}
