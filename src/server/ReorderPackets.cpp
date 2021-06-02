// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <SislTools/SislTools.hpp>
#include "ReorderPackets.hpp"
#include "StreamInterface.hpp"
#include <algorithm>
#include <iostream>
#include <queue>
#include <rapidjson/document.h>
#include <regex>
#include <optional>

ReorderPackets::ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength, std::uint32_t maxFilenameLength) :
  maxBufferSize(maxBufferSize),
  maxQueueLength(maxQueueLength),
  maxFilenameLength(maxFilenameLength)
{
}

bool ReorderPackets::write(
  std::istream& inputStream,
  StreamInterface* streamWrapper,
  std::uint32_t frameCount,
  bool eOFFlag)
{
  if (frameCount == nextFrameCount)
  {
    if (eOFFlag)
    {
      streamWrapper->setStoredFilename(getFilenameFromStream(inputStream).value_or("rejected."));
      return true;
    }
    streamWrapper->write(inputStream);
    ++nextFrameCount;
    return checkQueueAndSend(streamWrapper);
  }
  addFrameToQueue(inputStream, frameCount, eOFFlag);
  return false;
}

std::optional<std::string> ReorderPackets::getFilenameFromStream(std::istream& inputStream)
{
  std::string sislHeader;
  std::copy_if(std::istreambuf_iterator<char>(inputStream), std::istreambuf_iterator<char>(), std::back_inserter(sislHeader),
               [count = maxFilenameLength + 13](auto&&) mutable
               { return count && count--;});
  try
  {
    const auto filename = convertFromSisl(sislHeader);
    std::regex filter("[a-zA-Z0-9\\.\\-_]+");

    return std::regex_match(filename, filter) ? filename : std::optional<std::string>();
  }
  catch(std::exception&)
  {
    return std::optional<std::string>();
  }
}

std::string ReorderPackets::convertFromSisl(std::string sislFilename)
{
  if (sislFilename.find("\"}") == std::string::npos)
  {
    sislFilename = sislFilename + "\"}";
  }
  const auto json = SislTools::toJson(sislFilename);
  rapidjson::Document doc;
  doc.Parse(json.c_str());
  return doc.HasMember("name") ? doc["name"].GetString() : "";
}

bool ReorderPackets::checkQueueAndSend(StreamInterface* streamWrapper)
{
  while (!queue.empty() && (queue.top().frameCount == nextFrameCount))
  {
    if (queue.top().endOfFile)
    {
      queue.pop();
      return true;
    }
    streamWrapper->write(queue.top().frame);
    queue.pop();
    ++nextFrameCount;
  }
  return false;
}

void ReorderPackets::addFrameToQueue(std::istream& inputStream, std::uint32_t frameCount, bool endOfFile)
{
  if (queue.size() >= maxQueueLength)
  {
    std::cerr << "ReorderPackets: maxQueueLength exceeded." << std::endl;
    return;
  }

  std::vector<char> newFrame;
  newFrame.reserve(maxBufferSize);

  std::copy(
    std::istreambuf_iterator<char>(inputStream),
    std::istreambuf_iterator<char>(),
    std::back_inserter(newFrame));

  queue.emplace(FrameDetails{std::move(newFrame), frameCount, endOfFile});
}

