// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_STREAMSPY_HPP
#define ENTERPRISEDIODETESTER_STREAMSPY_HPP

#include "StreamInterface.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

class StreamSpy : public StreamInterface
{
public:
  explicit StreamSpy(std::stringstream& outputStream, std::uint32_t sessionId) :
    outputStream(outputStream),
    sessionId(sessionId)
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

  void setStoredFilename(std::string filename) override
  {
    storedFilename = (filename == "rejected?filename") ? "received." + std::to_string(sessionId) : filename;
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
  std::string storedFilename;
  const std::uint32_t sessionId;
};

#endif //ENTERPRISEDIODETESTER_STREAMSPY_HPP
