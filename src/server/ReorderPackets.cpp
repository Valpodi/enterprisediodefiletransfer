// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "ReorderPackets.hpp"
#include "StreamInterface.hpp"
#include <SislTools/SislTools.hpp>
#include <chrono>
#include <iostream>
#include <optional>
#include <rapidjson/document.h>
#include <regex>

ReorderPackets::ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength,
                               DiodeType diodeType, std::uint32_t maxFilenameLength) :
  maxBufferSize(maxBufferSize),
  maxQueueLength(maxQueueLength),
  maxFilenameLength(maxFilenameLength),
  diodeType(diodeType)
{
}

bool ReorderPackets::write(
  std::istream& inputStream,
  StreamInterface* streamWrapper,
  std::uint32_t frameCount,
  bool eOFFlag)
{
  logOutOfOrderPackets(frameCount);
  addFrameToQueue(inputStream, frameCount, eOFFlag);
  return checkQueueAndWrite(streamWrapper);
}

void ReorderPackets::logOutOfOrderPackets(uint32_t frameCount)
{
  if (frameCount != lastFrameReceived + 1)
  {
    std::cout << std::chrono::system_clock::now().time_since_epoch().count() << " Out of order frame: " << frameCount << "\n";
  }
  lastFrameReceived = frameCount;
}

std::optional<std::string> ReorderPackets::getFilenameFromStream(const BytesBuffer& eofFrame)
{
  const auto sislHeader = std::string(eofFrame.begin(), eofFrame.end());
  try
  {
    if (sislHeader.size() > maxSislLength)
    {
      std::cerr << "SISL too long" << "\n";
      return std::optional<std::string>();
    }
    const auto filename = convertFromSisl(sislHeader);
    if (filename.size() > maxFilenameLength)
    {
      std::cerr << "Filename too long" << "\n";
      return std::optional<std::string>();
    }
    std::regex filter("[a-zA-Z0-9\\.\\-_]+");

    return std::regex_match(filename, filter) ? filename : std::optional<std::string>();
  }
  catch (UnableToParseSislException&)
  {
    std::cerr << "Unable to parse SISL filename. possible regex problem" << "\n";
    return std::optional<std::string>();
  }
}

std::string ReorderPackets::convertFromSisl(std::string sislFilename)
{
  if (sislFilename.find("\"}") == std::string::npos)
  {
    sislFilename += "\"}";
  }
  const auto json = SislTools::toJson(sislFilename);
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  return doc.HasMember("name") ? doc["name"].GetString() : "";
}

bool ReorderPackets::checkQueueAndWrite(StreamInterface* streamWrapper)
{
  while (!queue.empty() && (queue.top().frameCount == nextFrameCount))
  {
    if (queue.top().endOfFile)
    {
      streamWrapper->setStoredFilename(getFilenameFromStream(queue.top().getFrame()).value_or("rejected."));
      queue.pop();
      return true;
    }
    writeFrame(streamWrapper);
    queue.pop();
    ++nextFrameCount;
  }
  return false;
}

void ReorderPackets::writeFrame(StreamInterface *streamWrapper)
{
  if (diodeType == DiodeType::import)
  {
    streamWrapper->write(streamingRewrapper.rewrap(queue.top().getFrame(), nextFrameCount));
  }
  else
  {
    streamWrapper->write(queue.top().getFrame());
  }
}

void ReorderPackets::addFrameToQueue(std::istream& inputStream, std::uint32_t frameCount, bool endOfFile)
{
  if (queue.size() >= maxQueueLength)
  {
    if (!queueAlreadyExceeded)
    {
      std::cerr << "ReorderPackets: maxQueueLength exceeded." << std::endl;
      queueAlreadyExceeded = true;
    }
    return;
  }

  BytesBuffer newFrame;
  newFrame.reserve(maxBufferSize);

  std::copy(
    std::istreambuf_iterator<char>(inputStream),
    std::istreambuf_iterator<char>(),
    std::back_inserter(newFrame));

  queue.emplace(FrameDetails{frameCount, endOfFile, std::move(newFrame)});
}

