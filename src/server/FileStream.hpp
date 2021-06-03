// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>

class FileStream : public StreamInterface
{
public:
  explicit FileStream(std::uint32_t sessionId) :
    sessionId(sessionId),
    tempFilename(setTempFilename())
  {
    outputStream.open(".received." + std::to_string(sessionId));
    outputStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
  }

  void deleteFile() override
  {
    outputStream.close();
    std::cerr << "Removing .received. file" << "\n";
    std::filesystem::remove(".received." + std::to_string(tempFilename));
  }

  void renameFile() override
  {
    outputStream.close();
    std::cout << "File complete. Renaming .received. file" << "\n";
    std::cout << storedFilename << "\n";
    std::filesystem::rename(".received." + std::to_string(tempFilename), storedFilename);
  }

  void setStoredFilename(std::string filename) override
  {
    storedFilename = (filename == "rejected.") ? filename+std::to_string(tempFilename) : filename;
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
  static uint32_t setTempFilename()
  {
    const auto seed = std::chrono::system_clock::now().time_since_epoch().count();
    return (std::uint32_t)std::default_random_engine(seed)();
  }

  const std::uint32_t sessionId;
  std::ofstream outputStream;
  std::string storedFilename;
  const std::uint32_t tempFilename;
};
