// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef UDPSERVERINTERFACE_HPP
#define UDPSERVERINTERFACE_HPP

#include <functional>
#include <boost/asio/buffer.hpp>

class UdpServerInterface
{
public:
  explicit UdpServerInterface() = default;

  virtual ~UdpServerInterface() = default;

  void setCallback(std::function<void(std::vector<std::uint8_t>&&, std::vector<std::uint8_t>&&)> requestedCallback)
  {
    callback = requestedCallback;
  }

protected:
  std::function<void(std::vector<std::uint8_t>&&, std::vector<std::uint8_t>&&)> callback;
};

#endif //UDPSERVERINTERFACE_HPP
