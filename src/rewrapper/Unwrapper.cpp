// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <cstdint>
#include <string>
#include <boost/filesystem/operations.hpp>
#include "StreamingFileInfo.hpp"
#include "Unwrapper.hpp"
#include "CloakedDagger.hpp"

void Unwrapper::unwrapFromStream(std::istream& inputStream, std::ostream& outputStream)
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

bool Unwrapper::isCloakDaggerEncoded(std::istream& inputStream) const
{
  return inputStream.peek() == CloakedDagger::cloakedDaggerIdentifierByte;
}

bool Unwrapper::isCloakDaggerEncoded(const BytesBuffer& inputData) const
{
  return inputData.size() != 0 &&
         CloakedDagger::cloakedDaggerIdentifierByte == inputData.at(0);
}


void Unwrapper::cloakDaggerUnwrap(std::istream& inputStream, std::ostream& outputStream) const
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


BytesBuffer Unwrapper::unwrap(const BytesBuffer& inputData)
{
  if (!isCloakDaggerEncoded(inputData))
  {
    return inputData;
  }

  const auto wrappedHeader = CloakedDagger::createFromBuffer(inputData);

  uint64_t count = 0;

  BytesBuffer unwrappedBytes;
  unwrappedBytes.reserve(inputData.size() - CloakedDagger::headerSize());

  for (size_t i=CloakedDagger::headerSize(); i<inputData.size(); i++)
  {
    unwrappedBytes.push_back(inputData[i] ^ static_cast<unsigned char>(wrappedHeader.key[count % CloakedDagger::maskLength]));
    count++;
  }

  return unwrappedBytes;
}

StreamingFileInfo Unwrapper::startFile(const StreamingFileInfo& info, const BytesBuffer&)
{
  return info;
}
