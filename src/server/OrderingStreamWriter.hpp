// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ORDERINGSTREAMWRITER_HPP
#define ORDERINGSTREAMWRITER_HPP

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
    bool dropPackets,
    std::unique_ptr<StreamInterface> stream,
    std::function<std::time_t()> getTime,
    DiodeType diodeType);

  bool write(Packet&& data);
  void deleteFile();
  void renameFile();

private:
  ReorderPackets packetQueue;
  std::unique_ptr<StreamInterface> streamWrapper;
  std::function<std::time_t()> getTime;

public:
  std::time_t timeLastUpdated;

private:
  DiodeType diodeType;
};

#endif //ORDERINGSTREAMWRITER_HPP
