// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <BytesBuffer.hpp>
#include <optional>
#include <queue>
#include <rewrapper/StreamingRewrapper.hpp>

class StreamInterface;

enum class diodeType
{
  import,
  basic
};

class ReorderPackets
{

public:
  explicit ReorderPackets(std::uint32_t maxBufferSize, std::uint32_t maxQueueLength,
    std::uint32_t maxFilenameLength = 65, diodeType diode = diodeType::basic);
  bool write(std::istream& inputStream, StreamInterface* streamWrapper, std::uint32_t frameCount, bool eOFFlag);

private:
  bool checkQueueAndWrite(StreamInterface* streamWrapper);
  void addFrameToQueue(std::istream& inputStream, std::uint32_t frameCount, bool endOfFile);
  std::optional<std::string> getFilenameFromStream(BytesBuffer eofFrame);
  std::string convertFromSisl(std::string sislFilename);

  struct FrameDetails {
    std::uint32_t frameCount;
    bool endOfFile;

    FrameDetails(std::uint32_t frameCount, bool endOfFile, BytesBuffer frame):
      frameCount(frameCount),
      endOfFile(endOfFile),
      frame(std::move(frame))
    {}

    bool operator>(const FrameDetails& rhs) const
    {
      return (frameCount > rhs.frameCount);
    }

    BytesBuffer getFrame() const
    {
      return frame;
    }

  private:
    BytesBuffer frame;
  };

  std::uint32_t nextFrameCount = 1;
  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  std::priority_queue<FrameDetails, std::vector<FrameDetails>, std::greater<>> queue;
  std::uint32_t maxFilenameLength;
  std::uint32_t maxSislLength = 1000;
  diodeType diode;
  StreamingRewrapper streamingRewrapper;
};
