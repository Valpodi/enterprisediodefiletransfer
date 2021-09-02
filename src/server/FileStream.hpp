// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "StreamInterface.hpp"
#include <algorithm>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <random>
#include "spdlog/spdlog.h"
#include "TotalFrames.hpp"

class FileStream : public StreamInterface
{
public:
  explicit FileStream(std::uint32_t sessionId) :
    sessionId(sessionId),
    tempFilename(setTempFilename())
  {
    const unsigned int outputBufferSize = 8192;
    char outputBuffer[outputBufferSize];
    
    outputStream.rdbuf()->pubsetbuf(outputBuffer, outputBufferSize);
    outputStream.open(".received." + std::to_string(tempFilename));
    outputStream.exceptions(std::ofstream::failbit | std::ofstream::badbit);
    spdlog::info("Session: " + std::to_string(sessionId) + " started");
  }

  void deleteFile() override
  {
    outputStream.close();
    spdlog::error("Removing .received. file");
    std::filesystem::remove(".received." + std::to_string(tempFilename));
  }

  void renameFile() override
  {
    outputStream.close();
    spdlog::info("File complete. Renaming .received. file" );
    spdlog::info(storedFilename);
    std::filesystem::rename(".received." + std::to_string(tempFilename), storedFilename);
    spdlog::info("Max size of Queue: " + std::to_string(queueUsagePeak));
    queueUsagePeak = 0;
  }

  void setStoredFilename(std::string filename) override
  {
    storedFilename = (filename == "rejected.") ? filename+std::to_string(tempFilename) : filename;
    spdlog::info("File: " + filename + " received");
  }

  void write(const BytesBuffer& inputData) override
  {
    //spdlog::info("writing bytes:");
    //for (int i = 0 ; i < 5; i++)
    //{
      spdlog::info(inputData.data()[0]);
    //}
    outputStream.write(reinterpret_cast<const char*>(inputData.data()), static_cast<long>(inputData.size()));
    outputStream.flush();
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
