// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_PARSING_HPP
#define ENTERPRISEDIODETESTER_PARSING_HPP

#include <cstdint>
#include <vector>

struct Parsing
{
  template <typename T, typename ARRAY>
  static T extract(const ARRAY &v, unsigned long pos)
  {
    T value;
    memcpy(&value, &v[static_cast<unsigned long>(pos)], sizeof(T));
    return value;
  }
};

#endif // ENTERPRISEDIODETESTER_PARSING_HPP
