// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <queue>

class StreamInterface;

class ReorderPackets
{

public:
  explicit ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength, std::uint32_t maxFilenameLength = 65);
  bool write(std::istream& inputStream, StreamInterface* streamWrapper, std::uint32_t frameCount, bool eOFFlag);

private:
  bool checkQueueAndSend(StreamInterface* streamWrapper);
  void addFrameToQueue(std::istream& inputStream, std::uint32_t frameCount, bool endOfFile);

  struct FrameDetails {
    std::vector<char> frame;
    std::uint32_t frameCount;
    bool endOfFile;

    bool operator>(const FrameDetails& rhs) const
    {
      return (frameCount > rhs.frameCount);
    }
  };

  std::uint32_t nextFrameCount = 1;
  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  std::priority_queue<FrameDetails, std::vector<FrameDetails>, std::greater<>> queue;

  std::string getFilenameFromStream(std::istream& inputStream);
  std::uint32_t maxFilenameLength;
};
