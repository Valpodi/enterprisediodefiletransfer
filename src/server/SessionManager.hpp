// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODE_SESSIONMANAGER_HPP
#define ENTERPRISEDIODE_SESSIONMANAGER_HPP

#include <map>
#include <set>
#include "OrderingStreamWriter.hpp"
#include "StreamInterface.hpp"

class SessionManager
{
public:
  explicit SessionManager(
    std::uint32_t maxBufferSize,
    std::uint32_t maxQueueLength,
    std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator,
    std::function<time_t()> getTime,
    std::uint32_t timeoutPeriod);

  void writeToStream(Packet packet);

private:
  void closeSession(std::uint32_t sessionId);
  void createNewSession(uint32_t sessionId);

  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  std::map<std::uint32_t, OrderingStreamWriter> streams;
  std::function<std::unique_ptr<StreamInterface>(std::uint32_t)> streamCreator;
  std::function<time_t()> getTime;
  std::uint32_t timeoutPeriod;
  void createSessionIfNewId(std::uint32_t sessionId);
  bool isStreamExpired(std::uint32_t sessionId);
  void writeFileAndSaveIfComplete(Packet packet);
};

#endif //ENTERPRISEDIODE_SESSIONMANAGER_HPP
