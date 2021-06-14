// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <BytesBuffer.hpp>
#include <optional>
#include <queue>
#include <rewrapper/StreamingRewrapper.hpp>
#include <algorithm>

class StreamInterface;
struct Packet;

enum class DiodeType
{
  import,
  basic
};

class ReorderPackets
{
public:
  explicit ReorderPackets(
    std::uint32_t maxBufferSize,
    std::uint32_t maxQueueLength,
    bool dropPackets,
    DiodeType diodeType,
    std::uint32_t maxFilenameLength = 65);
  bool write(Packet&& packet, StreamInterface* streamWrapper);

private:
  bool checkQueueAndWrite(StreamInterface* streamWrapper);
  void addFrameToQueue(std::vector<std::uint8_t>&& inputStream, std::uint32_t frameCount, bool endOfFile);
  std::optional<std::string> getFilenameFromStream(const BytesBuffer& eofFrame);
  std::string convertFromSisl(std::string sislFilename);

  struct FrameDetails {
    std::uint32_t frameCount;
    bool endOfFile;

    FrameDetails(std::uint32_t frameCount, bool endOfFile, BytesBuffer&& frame):
      frameCount(frameCount),
      endOfFile(endOfFile),
      frame(std::move(frame))
    {}

    FrameDetails(const FrameDetails&) = delete;
    FrameDetails& operator=(FrameDetails&) = delete;

    FrameDetails& operator=(FrameDetails&& rhs)
    {
      frameCount = std::move(rhs.frameCount);
      endOfFile = std::move(rhs.endOfFile);
      frame = std::move(rhs.frame);
      return *this;
    }

    FrameDetails(FrameDetails&& rhs) :
     frameCount(rhs.frameCount),
     endOfFile(rhs.endOfFile),
     frame(std::move(rhs.frame))
    {
    };

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

  bool queueAlreadyExceeded = false;
  std::uint32_t nextFrameCount = 1;
  std::uint32_t lastFrameReceived = 0;
  std::uint32_t maxBufferSize;
  std::uint32_t maxQueueLength;
  bool dropPackets;
  std::priority_queue<FrameDetails, std::vector<FrameDetails>, std::greater<>> queue;
  std::uint32_t maxFilenameLength;
  std::uint32_t maxSislLength = 1000;
  DiodeType diodeType;
  StreamingRewrapper streamingRewrapper;
  void writeFrame(StreamInterface *streamWrapper);
  void logOutOfOrderPackets(uint32_t frameCount);
  BytesBuffer copyToBuffer(std::istream& inputStream) const;
  BytesBuffer createBuffer() const;
  BytesBuffer& copy(std::istream& inputStream, BytesBuffer&& newFrame) const;
  static FrameDetails getDetails(std::vector<std::uint8_t>&& inputStream, uint32_t frameCount, bool endOfFile) ;
};
