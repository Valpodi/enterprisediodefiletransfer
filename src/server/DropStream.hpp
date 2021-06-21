// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"
#include "spdlog/spdlog.h"

class DropStream: public StreamInterface
{
public:
  explicit DropStream(std::uint32_t sessionId)
  {
    spdlog::info("Session: " + std::to_string(sessionId) + " started");
  }
  void deleteFile() override
  {
    spdlog::error("File incomplete. Dropping stream");
  }
  void renameFile() override
  {
    spdlog::info("Transfer complete");
  }
  void setStoredFilename(std::string filename) override
  {
    spdlog::info("File: " + filename + " received");
  }
  void write(const BytesBuffer&) override { }
};