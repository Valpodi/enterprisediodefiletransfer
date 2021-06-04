// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "MessageParsingHelpers.hpp"
#include "UnwrapperTestHelpers.hpp"


std::stringstream createTestWrappedString(const std::string& payload, const BytesBuffer& mask)
{
  if (mask.size() != 8)
  {
    throw std::runtime_error("Bad mask length");
  }

  auto bytes = BytesBuffer({0xd1, 0xdf, 0x5f, 0xff, // magic1
                            0x00, 0x01, // major version
                            0x00, 0x00, // minor version
                            0x00, 0x00, 0x00, 0x30, // total length
                            0x00, 0x00, 0x00, 0x01, // encoding type
                            0x00, 0x03, // encoding config
                            0x00, 0x08, // encoding data length
                             // mask will be here
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 1
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 2
                            0xff, 0x5f, 0xdf, 0xd1});  // magic2
  bytes.insert(bytes.begin() + 20, mask.begin(), mask.end());

  auto messageStream = std::stringstream(std::string(bytes.begin(), bytes.end()));
  messageStream.seekp(0, std::ios::end);

  auto count = 0;
  for (auto payloadChar: payload)
  {
    messageStream.put(static_cast<char>(payloadChar ^ mask[count++ % mask.size()]));
  }

  return messageStream;
}

BytesBuffer createTestWrappedBytesBuffer(const std::string& payload, const BytesBuffer& mask)
{
  return MessageParsingHelpers::StringToBytes(createTestWrappedString(payload, mask).str());
}

BytesBuffer dummyWrappedBuffer()
{
  return BytesBuffer{0xd1};
}
