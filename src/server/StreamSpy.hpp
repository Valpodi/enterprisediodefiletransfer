// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ENTERPRISEDIODETESTER_STREAMSPY_HPP
#define ENTERPRISEDIODETESTER_STREAMSPY_HPP

#include "StreamInterface.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>

class StreamSpy: public StreamInterface
{
public:
  explicit StreamSpy(
    std::stringstream& outputStream,
    std::uint32_t sessionId,
    bool& fileDeletedWasCalled,
    bool& fileRenameWasCalled):
      outputStream(outputStream),
      sessionId(sessionId),
      tempFilename(setTempFilename()),
      fileDeletedWasCalled(fileDeletedWasCalled),
      fileRenameWasCalled(fileRenameWasCalled)
  {
  }

  explicit StreamSpy(
    std::stringstream& outputStream,
    std::uint32_t sessionId):
    outputStream(outputStream),
    sessionId(sessionId),
    tempFilename(setTempFilename()),
    fileDeletedWasCalled(notused1),
    fileRenameWasCalled(notused2)
  {
  }

  void deleteFile() override { fileDeletedWasCalled = true; }

  void renameFile() override { fileRenameWasCalled = true; }

  void setStoredFilename(std::string filename) override
  {
    storedFilename = (filename == "rejected.") ? filename + std::to_string(tempFilename) : filename;
  }

  void write(const BytesBuffer& inputData) override
  {
    std::copy(inputData.begin(), inputData.end(), std::ostreambuf_iterator(outputStream));
  }

public:
  std::stringstream& outputStream;
  std::string storedFilename;
  const std::uint32_t sessionId;
  const std::uint32_t tempFilename;
  bool& fileDeletedWasCalled;
  bool& fileRenameWasCalled;

  bool notused1;
  bool notused2;

private:
  static uint32_t setTempFilename() { return 12345; }
};

#endif // ENTERPRISEDIODETESTER_STREAMSPY_HPP
