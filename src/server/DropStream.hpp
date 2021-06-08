// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"

class DropStream: public StreamInterface
{
public:
  explicit DropStream(std::uint32_t) {}
  void deleteFile() override {}
  void renameFile() override
  {
    std::cout << "Transfer complete" << "\n";
  }
  void setStoredFilename(std::string) override { }
  void write(const BytesBuffer&) override { }
};