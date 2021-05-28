// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODE_STREAMINTERFACE_HPP
#define ENTERPRISEDIODE_STREAMINTERFACE_HPP

#include <vector>

class StreamInterface
{
public:
  virtual ~StreamInterface() = default;
  virtual void deleteFile() = 0;
  virtual void renameFile() = 0;
  virtual void setStoredFilename(std::istream& inputData) = 0;
  virtual void write(const std::vector<char>& inputData) = 0;
  virtual void write(std::istream& inputStream) = 0;
};

#endif //ENTERPRISEDIODE_STREAMINTERFACE_HPP
