// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "UnwrapperTestHelpers.hpp"
#include "CloakedDagger.hpp"
#include "MessageParsingHelpers.hpp"
#include <algorithm>
#include <iostream>

TestPacket createTestWrappedString(const std::string& payload, const std::array<char, 8>& mask)
{
  if (mask.size() != 8)
  {
    throw std::runtime_error("Bad mask length");
  }

  auto header = std::array<char, CloakedDagger::headerSize()>({static_cast<char>(0xd1), static_cast<char>(0xdf), 0x5f, static_cast<char>(0xff), // magic1
                            0x00, 0x01, // major version
                            0x00, 0x00, // minor version
                            0x00, 0x00, 0x00, 0x30, // total length
                            0x00, 0x00, 0x00, 0x01, // encoding type
                            0x00, 0x03, // encoding config
                            0x00, 0x08, // encoding data length
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // mask will be here
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 1
                            0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // header 2
     static_cast<char>(0xff), 0x5f, static_cast<char>(0xdf), static_cast<char>(0xd1)});  // magic2
  std::copy(mask.begin(), mask.end(), header.begin() + 20);

  BytesBuffer messageStream;

  unsigned long count = 0;
  for (auto payloadChar: payload)
  {
    messageStream.push_back((static_cast<unsigned char>(payloadChar ^ mask[count++ % mask.size()])));
  }

  return {header, messageStream};
}

bool isCloakDaggerEncoded(std::istream& inputStream)
{
  return inputStream.peek() == CloakedDagger::cloakedDaggerIdentifierByte;
}

void cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream)
{
  std::array<char, 48> headerBuffer{};
  std::copy_n(std::istreambuf_iterator<char>(inputStream), 49, headerBuffer.begin());

  const CloakedDagger wrappedHeader(headerBuffer);

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
