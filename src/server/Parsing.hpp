// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_PARSING_HPP
#define ENTERPRISEDIODETESTER_PARSING_HPP

#include <cstdint>
#include <vector>

struct Parsing
{
  template <typename T>
  static T extract(const std::vector<std::uint8_t> &v, int pos)
  {
    T value;
    memcpy(&value, &v[pos], sizeof(T));
    return value;
  }
};

#endif // ENTERPRISEDIODETESTER_PARSING_HPP
