// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_EDCLIENTWRAPPER_HPP
#define ENTERPRISEDIODE_EDCLIENTWRAPPER_HPP

#include <iostream>
#include <fstream>
#include "Client.hpp"
#include "UdpClient.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"

class ClientWrapper
{
public:
  ClientWrapper(const std::string& targetAddress,
    std::uint16_t targetPort,
    std::uint16_t mtuSize,
    double dataRateMbps,
    std::string filename);
  void sendData(const std::string& filename);

private:
  Client edClient;

};

std::uint16_t calculatePayloadSize(std::uint16_t mtuSize);

#endif //ENTERPRISEDIODE_EDCLIENTWRAPPER_HPP
