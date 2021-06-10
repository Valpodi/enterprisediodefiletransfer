// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"

class DropStream: public StreamInterface
{
public:
  explicit DropStream(std::uint32_t sessionId)
  {
    std::cout << "Session: " << sessionId << " started\n";
  }
  void deleteFile() override
  {
    std::cerr << "File incomplete. Dropping stream" << "\n";
  }
  void renameFile() override
  {
    std::cout << "Transfer complete" << "\n";
  }
  void setStoredFilename(std::string filename) override
  {
    std::cout << "File: " << filename << " received" << "\n";
  }
  void write(const BytesBuffer&) override { }
};