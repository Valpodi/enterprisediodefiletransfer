// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_UDPCLIENTINTERFACE_HPP
#define ENTERPRISEDIODE_UDPCLIENTINTERFACE_HPP

#include <boost/asio/buffer.hpp>

using ConstSocketBuffers = std::array<boost::asio::const_buffer, 2>;


class UdpClientInterface
{
public:
  virtual ~UdpClientInterface() = default;

  virtual void send(ConstSocketBuffers inputBuffers) = 0;
};

#endif //ENTERPRISEDIODE_UDPCLIENTINTERFACE_HPP
