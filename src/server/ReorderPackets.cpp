// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <SislTools/SislTools.hpp>
#include "ReorderPackets.hpp"
#include "StreamInterface.hpp"
#include <iostream>
#include <rapidjson/document.h>
#include <regex>
#include <optional>

ReorderPackets::ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength,
  std::uint32_t maxFilenameLength, diodeType diode) :
  maxBufferSize(maxBufferSize),
  maxQueueLength(maxQueueLength),
  maxFilenameLength(maxFilenameLength),
  diode(diode)
{
}

bool ReorderPackets::write(
  std::istream& inputStream,
  StreamInterface* streamWrapper,
  std::uint32_t frameCount,
  bool eOFFlag)
{
  addFrameToQueue(inputStream, frameCount, eOFFlag);
  return checkQueueAndWrite(streamWrapper);
}

std::optional<std::string> ReorderPackets::getFilenameFromStream(BytesBuffer eofFrame)
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
    auto data = queue.top().getFrame();
    if (queue.top().endOfFile)
    {
      streamWrapper->setStoredFilename(getFilenameFromStream(data).value_or("rejected."));
      queue.pop();
      return true;
    }
    if (diode == diodeType::import)
    {
      data = streamingRewrapper.rewrap(data, nextFrameCount);
    }
    streamWrapper->write(data);
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

  BytesBuffer newFrame;
  newFrame.reserve(maxBufferSize);

  std::copy(
    std::istreambuf_iterator<char>(inputStream),
    std::istreambuf_iterator<char>(),
    std::back_inserter(newFrame));

  queue.emplace(FrameDetails{frameCount, endOfFile, std::move(newFrame)});
}

