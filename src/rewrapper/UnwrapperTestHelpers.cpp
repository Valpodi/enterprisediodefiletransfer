// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "MessageParsingHelpers.hpp"
#include "UnwrapperTestHelpers.hpp"
#include "CloakedDagger.hpp"


std::string createTestWrappedString(const std::string& payload, const BytesBuffer& mask)
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

  auto messageStream = std::string(bytes.begin(), bytes.end());

  unsigned long count = 0;
  for (auto payloadChar: payload)
  {
    messageStream.push_back(static_cast<char>(payloadChar ^ mask[count++ % mask.size()]));
  }

  return messageStream;
}

BytesBuffer createTestWrappedBytesBuffer(const std::string& payload, const BytesBuffer& mask)
{
  return MessageParsingHelpers::StringToBytes(createTestWrappedString(payload, mask));
}

bool isCloakDaggerEncoded(std::istream& inputStream)
{
  return inputStream.peek() == CloakedDagger::cloakedDaggerIdentifierByte;
}

void cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream)
{
  const CloakedDagger wrappedHeader(inputStream);

  char readByte;
  uint64_t count = 0;

  while (!inputStream.get(readByte).eof())
  {
    outputStream.put(readByte ^ static_cast<char>(wrappedHeader.key[count % CloakedDagger::maskLength]));
    count++;
  }
}

void unwrapFromStream(std::istream& inputStream, std::ostream& outputStream)
{
  if (isCloakDaggerEncoded(inputStream))
  {
    cloakDaggerUnwrap(inputStream, outputStream);
  }
  else
  {
    outputStream << inputStream.rdbuf();
  }
}
