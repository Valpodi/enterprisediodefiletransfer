// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef UDPCLIENTINTERFACE_HPP
#define UDPCLIENTINTERFACE_HPP

#include <boost/asio/buffer.hpp>

using ConstSocketBuffers = std::array<boost::asio::const_buffer, 2>;


class UdpClientInterface
{
public:
  virtual ~UdpClientInterface() = default;

  virtual void send(ConstSocketBuffers inputBuffers) = 0;
};

#endif //UDPCLIENTINTERFACE_HPP
