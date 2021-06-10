// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamingRewrapper.hpp"
#include "CloakedDagger.hpp"
#include "BytesBuffer.hpp"


BytesBuffer StreamingRewrapper::rewrap(const BytesBuffer& input, std::uint32_t frameCount)
{
  if (input.at(0) != CloakedDagger::cloakedDaggerIdentifierByte)
  {
    return input;
  }
  const auto inputChunkMask = getMaskFromHeader(input);

  if (frameCount == 1)
  {
    return handleFirstFrame(input, inputChunkMask);
  }
  return rewrapData(input, constructXORedMask(inputChunkMask));
}

BytesBuffer StreamingRewrapper::handleFirstFrame(const BytesBuffer& input, const BytesBuffer& inputChunkMask)
{
  mask = inputChunkMask;
  mask_index = input.size() - CloakedDagger::headerSize();
  return input;
}

BytesBuffer StreamingRewrapper::rewrapData(const BytesBuffer& input, const BytesBuffer& newMask)
{
  BytesBuffer output;

  for (auto c=input.begin() + CloakedDagger::headerSize(); c != input.end(); c++)
  {
    output.push_back(*c ^ newMask.at(mask_index % CloakedDagger::maskLength));
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

BytesBuffer StreamingRewrapper::getMaskFromHeader(const BytesBuffer& input)
{
  const auto header = CloakedDagger::createFromBuffer(input);

  return BytesBuffer(header.key.begin(), header.key.end());
}
