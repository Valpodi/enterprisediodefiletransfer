// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_UNWRAPPERTESTHELPERS_HPP
#define REWRAPPER_UNWRAPPERTESTHELPERS_HPP

#include <filesystem>
#include <BytesBuffer.hpp>
#include "Unwrapper.hpp"
#include <sstream>
#include "StreamingFileInfo.hpp"

struct DataPackage;

std::stringstream createTestWrappedString(const std::string& payload, const BytesBuffer& mask={0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
BytesBuffer createTestWrappedBytesBuffer(const std::string& payload, const BytesBuffer& mask={0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0});
BytesBuffer getMask(const BytesBuffer& data);
BytesBuffer dummyWrappedBuffer();

inline bool operator==(const ResumeInfo& a, const ResumeInfo& b)
{
  return a.contentRange == b.contentRange &&
         a.sessionID == b.sessionID;
}

inline bool operator==(const StreamingFileInfo& a, const StreamingFileInfo& b)
{
  return a.filename == b.filename &&
         a.contentLength == b.contentLength &&
         a.resumeInfo == b.resumeInfo &&
         a.destinationSpecifier == b.destinationSpecifier;
}


#endif //REWRAPPER_UNWRAPPERTESTHELPERS_HPP
