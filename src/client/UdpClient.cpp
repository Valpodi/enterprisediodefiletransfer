// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <boost/asio/ip/udp.hpp>
#include "UdpClient.hpp"

UdpClient::UdpClient(const std::string& address, std::uint16_t port) :
  io_context(),
  s(io_context, boost::asio::ip::udp::endpoint(boost::asio::ip::udp::v4(), 0)),
  endpoints(findEndpoints(address, port))
{

}

boost::asio::ip::udp::endpoint UdpClient::findEndpoints(const std::string& address, std::uint16_t port)
{
  boost::asio::ip::udp::resolver resolver(io_context);
  return *resolver.resolve({boost::asio::ip::udp::v4(), address.c_str(), std::to_string(port).c_str()});
}

void UdpClient::send(ConstSocketBuffers inputBuffers)
{
  s.send_to(inputBuffers, endpoints);
}
