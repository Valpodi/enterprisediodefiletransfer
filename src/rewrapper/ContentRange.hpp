// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_CONTENTRANGE_HPP
#define REWRAPPER_CONTENTRANGE_HPP

#include <cstdint>

struct ContentRange
{
  std::uint64_t start;
  std::uint64_t end;
  std::uint64_t total;
};

inline bool operator==(const ContentRange& a, const ContentRange& b)
{
  return a.start == b.start &&
    a.end == b.end &&
    a.total == b.total;
}

#endif //REWRAPPER_CONTENTRANGE_HPP
