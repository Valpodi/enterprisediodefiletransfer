// Copyright PA Knowledge Ltd 2021
// For licence terms see LICENCE.md file

#ifndef ENTERPRISEDIODETESTER_STREAMSPY_HPP
#define ENTERPRISEDIODETESTER_STREAMSPY_HPP

#include <fstream>
#include <filesystem>
#include "StreamInterface.hpp"

class StreamSpy : public StreamInterface
{
public:
  explicit StreamSpy(std::stringstream& outputStream) :
    outputStream(outputStream)
  {
  }

  void deleteFile() override
  {
    fileDeletedWasCalled = true;
  }

  void renameFile() override
  {
    fileRenameWasCalled = true;
  }

  void write(const std::vector<char>& inputData) override
  {
    std::copy(
      inputData.begin(),
      inputData.end(),
      std::ostreambuf_iterator(outputStream));
  }

  void write(std::istream& inputStream) override
  {
    std::copy(
      std::istreambuf_iterator<char>(inputStream),
      std::istreambuf_iterator<char>(),
      std::ostreambuf_iterator(outputStream));
  }

public:
  std::stringstream& outputStream;
  bool fileDeletedWasCalled = false;
  bool fileRenameWasCalled = false;
};

#endif //ENTERPRISEDIODETESTER_STREAMSPY_HPP
