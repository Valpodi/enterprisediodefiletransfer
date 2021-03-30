// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_ORDERINGSTREAMWRITER_HPP
#define ENTERPRISEDIODE_ORDERINGSTREAMWRITER_HPP

#include <memory>
#include <fstream>
#include <functional>
#include <ctime>
#include "ReorderPackets.hpp"
#include "diodeheader/EnterpriseDiodeHeader.hpp"
#include "StreamInterface.hpp"

class OrderingStreamWriter
{
public:
  explicit OrderingStreamWriter(
    std::uint32_t maxBufferSize,
    std::uint32_t maxQueueLength,
    std::unique_ptr<StreamInterface> stream,
    std::function<std::time_t()> getTime);

  bool write(std::istream& data, const HeaderParams& edHeader);
  void deleteFile();
  void renameFile();

private:
  ReorderPackets packetQueue;
  std::unique_ptr<StreamInterface> streamWrapper;
  std::function<std::time_t()> getTime;

public:
  std::time_t timeLastUpdated;
};

#endif //ENTERPRISEDIODE_ORDERINGSTREAMWRITER_HPP
