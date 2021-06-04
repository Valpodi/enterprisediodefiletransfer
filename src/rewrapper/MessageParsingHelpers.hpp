// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_MESSAGEPARSINGHELPERS_HPP
#define REWRAPPER_MESSAGEPARSINGHELPERS_HPP

#include <cstdint>
#include <string>
#include <vector>
#include "DataPackage.hpp"



struct MessageParsingHelpers
{
  static BytesBuffer StringToBytes(const std::string& inputString)
  {
    BytesBuffer rawByteVector;
    std::copy(inputString.begin(), inputString.end(), back_inserter(rawByteVector));
    return rawByteVector;
  }

};


#endif //REWRAPPER_MESSAGEPARSINGHELPERS_HPP
