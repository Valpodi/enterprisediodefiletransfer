// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.


#ifndef REWRAPPER_STREAMINGREWRAPPER_HPP
#define REWRAPPER_STREAMINGREWRAPPER_HPP

#include <boost/optional.hpp>
#include "CloakedDagger.hpp"

class StreamingRewrapper
{
public:
  StreamingRewrapper() = default;
  BytesBuffer rewrap(const BytesBuffer& input, const std::array<char, CloakedDagger::headerSize()>& cloakedDaggerHeader, std::uint32_t frameCount);

private:
  size_t mask_index {0};
  BytesBuffer mask = BytesBuffer(CloakedDagger::maskLength);

  static BytesBuffer getMaskFromHeader(const std::array<char, CloakedDagger::headerSize()>& cloakedDaggerHeader);
  BytesBuffer constructXORedMask(const BytesBuffer& inputChunkMask) const;
  BytesBuffer rewrapData(const BytesBuffer& input, const BytesBuffer& newMask);
  void handleFirstFrame(const BytesBuffer& input, const BytesBuffer& inputChunkMask);
};


#endif //REWRAPPER_STREAMINGREWRAPPER_HPP
