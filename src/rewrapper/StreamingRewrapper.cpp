// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamingRewrapper.hpp"
#include "BytesBuffer.hpp"
#include "CloakedDagger.hpp"

BytesBuffer StreamingRewrapper::rewrap(const BytesBuffer& input, const CloakedDaggerHeader& cloakedDaggerHeader, std::uint32_t frameCount)
{
  if (cloakedDaggerHeader.at(0) != static_cast<char>(CloakedDagger::cloakedDaggerIdentifierByte))
  {
    if (input.at(0) != '{' && input.at(0) != 'B')
        {
          throw std::runtime_error("received data that was not wrapped, sisl nor bitmap!");
        }
    return input;
  }
  const auto inputChunkMask = getMaskFromHeader(cloakedDaggerHeader);

  if (frameCount == 1)
  {
    handleFirstFrame(input, inputChunkMask);
    BytesBuffer firstFrame{cloakedDaggerHeader.begin(), cloakedDaggerHeader.end()};
    firstFrame.insert(firstFrame.end(), input.begin(), input.end());
    return firstFrame;
  }
  return rewrapData(input, constructXORedMask(inputChunkMask));
}

void StreamingRewrapper::handleFirstFrame(const BytesBuffer& input, const BytesBuffer& inputChunkMask)
{
  mask = inputChunkMask;
  mask_index = input.size();
}

BytesBuffer StreamingRewrapper::rewrapData(const BytesBuffer& input, const BytesBuffer& newMask)
{
  BytesBuffer output;

  for (unsigned char c : input)
  {
    output.push_back(c ^ newMask.at(mask_index % CloakedDagger::maskLength));
    mask_index++;
  }
  return output;
}

BytesBuffer StreamingRewrapper::constructXORedMask(const BytesBuffer& inputChunkMask) const
{
  if (mask == BytesBuffer(CloakedDagger::maskLength))
  {
    throw std::runtime_error("Tried to rewrap a frame before mask set.");
  }
  BytesBuffer newMask(CloakedDagger::maskLength);

  for (std::uint8_t rotatingInputIndex=0; rotatingInputIndex < CloakedDagger::maskLength; rotatingInputIndex++)
  {
    const auto rotatingOutputIndex = (rotatingInputIndex + mask_index) % CloakedDagger::maskLength;
    newMask[rotatingOutputIndex] = inputChunkMask.at(rotatingInputIndex) ^ mask.at(rotatingOutputIndex);
  }
  return newMask;
}

BytesBuffer StreamingRewrapper::getMaskFromHeader(const CloakedDaggerHeader& cloakedDaggerHeader)
{
  const auto header = CloakedDagger::createFromBuffer(cloakedDaggerHeader);

  return BytesBuffer(header.key.begin(), header.key.end());
}
