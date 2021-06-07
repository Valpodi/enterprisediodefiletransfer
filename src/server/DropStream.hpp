// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"

class DropStream: public StreamInterface
{
public:
  explicit DropStream(std::uint32_t sessionId) { (void)sessionId; }

  void deleteFile() override {}

  void renameFile() override
  {
    std::cout << "Transfer complete" << "\n";
  }

  void setStoredFilename(std::string filename) override { (void) filename; }
  void write(const std::vector<char>& inputData) override { (void) inputData; }
  void write(std::istream& inputStream) override { (void) inputStream; }
};