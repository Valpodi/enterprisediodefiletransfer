// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODE_UDPCLIENT_HPP
#define ENTERPRISEDIODE_UDPCLIENT_HPP

#include <boost/asio/ip/udp.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/asio/buffer.hpp>
#include "UdpClientInterface.hpp"


class UdpClient : public UdpClientInterface
{
public:
  explicit UdpClient(const std::string& address, std::uint16_t port);
  void send(ConstSocketBuffers inputBuffers) override;

private:
  boost::asio::io_service io_context;
  boost::asio::ip::udp::socket s;
  boost::asio::ip::udp::endpoint endpoints;

  boost::asio::ip::udp::endpoint findEndpoints(const std::string& address, std::uint16_t port);
};

#endif //ENTERPRISEDIODE_UDPCLIENT_HPP