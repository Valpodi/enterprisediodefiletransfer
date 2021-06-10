// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef STREAMINTERFACE_HPP
#define STREAMINTERFACE_HPP

#include <vector>
#include <BytesBuffer.hpp>

class StreamInterface
{
public:
  virtual ~StreamInterface() = default;
  virtual void deleteFile() = 0;
  virtual void renameFile() = 0;
  virtual void setStoredFilename(std::string filename) = 0;
  virtual void write(const BytesBuffer& inputData) = 0;
};

#endif //STREAMINTERFACE_HPP
