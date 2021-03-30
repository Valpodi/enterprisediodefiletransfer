// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_UDPSERVERINTERFACE_HPP
#define ENTERPRISEDIODE_UDPSERVERINTERFACE_HPP

#include <functional>
#include <boost/asio/buffer.hpp>

class UdpServerInterface
{
public:
  explicit UdpServerInterface() = default;

  virtual ~UdpServerInterface() = default;

  void setCallback(std::function<void(std::istream&)> requestedCallback)
  {
    callback = requestedCallback;
  }

protected:
  std::function<void(std::istream&)> callback;
};

#endif //ENTERPRISEDIODE_UDPSERVERINTERFACE_HPP
