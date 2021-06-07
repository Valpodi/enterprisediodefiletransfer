// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.


#ifndef REWRAPPER_STREAMINGREWRAPPER_HPP
#define REWRAPPER_STREAMINGREWRAPPER_HPP

#include <boost/optional.hpp>
#include "WrapperInterface.hpp"

class StreamingRewrapper: public WrapperInterface
{
public:
  StreamingRewrapper() = default;
  BytesBuffer unwrap(const BytesBuffer& data) override;

private:
  size_t count {0};
  BytesBuffer mask;

  BytesBuffer rewrap(const BytesBuffer& input);
  static BytesBuffer getMaskFromHeader(const BytesBuffer& input);
  BytesBuffer constructXORedMask(const BytesBuffer& inputChunkMask) const;
  BytesBuffer rewrapData(const BytesBuffer& input, const BytesBuffer& newMask);
};


#endif //REWRAPPER_STREAMINGREWRAPPER_HPP
