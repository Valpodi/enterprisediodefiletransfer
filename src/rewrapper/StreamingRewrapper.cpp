// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamingRewrapper.hpp"
#include "CloakedDagger.hpp"
#include "StreamingFileInfo.hpp"
#include "DataPackage.hpp"

BytesBuffer StreamingRewrapper::unwrap(const BytesBuffer& data)
{
  if (data.at(0) != CloakedDagger::cloakedDaggerIdentifierByte)
  {
    return data;
  }

  if (fileStart)
  {
    mask = getMaskFromHeader(data);
    fileStart = false;
    count = data.size() - CloakedDagger::headerSize();
    return data;
  }
  else
  {
    return rewrap(data);
  }
}

BytesBuffer StreamingRewrapper::rewrap(const BytesBuffer& input)
{
  const auto inputChunkMask = getMaskFromHeader(input);

  const BytesBuffer newMask = constructXORedMask(inputChunkMask);

  return rewrapData(input, newMask);
}

BytesBuffer StreamingRewrapper::rewrapData(const BytesBuffer& input, const BytesBuffer& newMask)
{
  BytesBuffer output;

  for (auto c=input.begin() + CloakedDagger::headerSize(); c != input.end(); c++)
  {
    output.push_back(*c ^ newMask.at(count % CloakedDagger::maskLength));
    count++;
  }
  return output;
}

BytesBuffer StreamingRewrapper::constructXORedMask(const BytesBuffer& inputChunkMask) const
{
  BytesBuffer newMask(CloakedDagger::maskLength);

  for (std::uint8_t rotatingInputIndex=0; rotatingInputIndex < CloakedDagger::maskLength; rotatingInputIndex++)
  {
    const auto rotatingOutputIndex = (rotatingInputIndex + count) % CloakedDagger::maskLength;
    newMask[rotatingOutputIndex] = inputChunkMask.at(rotatingInputIndex) ^ mask.at(rotatingOutputIndex);
  }
  return newMask;
}

BytesBuffer StreamingRewrapper::getMaskFromHeader(const BytesBuffer& input)
{
  const auto header = CloakedDagger::createFromBuffer(input);

  return BytesBuffer(header.key.begin(), header.key.end());
}

StreamingFileInfo StreamingRewrapper::startFile(const StreamingFileInfo& info, const BytesBuffer& firstData)
{
  if (info.resumeInfo)
  {
    resumableFileStart(info);
  }
  else
  {
    // Start of only segment of non-resumable file
    fileStart = true;
    currentSessionID = boost::none;
  }

  return updateFileInfo(info, firstData);
}

void StreamingRewrapper::resumableFileStart(const StreamingFileInfo& info)
{
  if (info.resumeInfo->contentRange.start == 0)
  {
    // Start of first segment of resumable file
    fileStart = true;
    currentSessionID = info.resumeInfo->sessionID;
  }
  else
  {
    if (info.resumeInfo->sessionID != currentSessionID)
    {
      throw std::runtime_error("starting re-wrapper with content-range not equal to zero");
    }

    if (fileStart)
    {
      throw std::logic_error("StreamingRewrapper::startFile called twice with no chunk");
    }

    count = info.resumeInfo->contentRange.start;
  }
}

StreamingFileInfo StreamingRewrapper::updateFileInfo(const StreamingFileInfo& info, const BytesBuffer& firstData) const
{
  StreamingFileInfo newInfo = info;

  if (firstData.at(0) != CloakedDagger::cloakedDaggerIdentifierByte)
  {
    return newInfo;
  }

  if (fileStart)
  {
    // First chunk of file, or first chunk of first segment of resumable file
    if (info.contentLength)
    {
      newInfo.contentLength = *newInfo.contentLength + CloakedDagger::headerSize();
    }

    if (info.resumeInfo)
    {
      newInfo.resumeInfo->contentRange = ContentRange{
        info.resumeInfo->contentRange.start,
        info.resumeInfo->contentRange.end + CloakedDagger::headerSize(),
        info.resumeInfo->contentRange.total + CloakedDagger::headerSize()};
    }
  }
  else
  {
    if (info.resumeInfo)
    {
      newInfo.resumeInfo->contentRange = ContentRange{
        info.resumeInfo->contentRange.start + CloakedDagger::headerSize(),
        info.resumeInfo->contentRange.end + CloakedDagger::headerSize(),
        info.resumeInfo->contentRange.total + CloakedDagger::headerSize()};
    }
  }

  return newInfo;
}
