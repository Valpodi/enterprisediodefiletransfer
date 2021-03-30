// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <fstream>
#include <filesystem>
#include "StreamInterface.hpp"

class FileStream : public StreamInterface
{
public:
  explicit FileStream(std::uint32_t sessionId) :
    sessionId(sessionId)
  {
    outputStream.open(".received." + std::to_string(sessionId));
    outputStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  }

  void deleteFile() override
  {
    outputStream.close();
    std::cerr << "Removing .received. file" << "\n";
    std::filesystem::remove(".received." + std::to_string(sessionId));
  }

  void renameFile() override
  {
    outputStream.close();
    std::cout << "File complete. Renaming .received. file" << "\n";
    std::filesystem::rename(".received." + std::to_string(sessionId), "received." + std::to_string(sessionId));
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

private:
  const std::uint32_t sessionId;
  std::ofstream outputStream;
};
