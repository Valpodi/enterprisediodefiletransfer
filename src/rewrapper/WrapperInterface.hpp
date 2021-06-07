// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_WRAPPERINTERFACE_HPP
#define REWRAPPER_WRAPPERINTERFACE_HPP

#include <string>
#include "BytesBuffer.hpp"

class WrapperInterface
{
public:
  virtual ~WrapperInterface() = default;
  virtual BytesBuffer unwrap(const BytesBuffer& data) = 0;
};

#endif //REWRAPPER_WRAPPERINTERFACE_HPP
