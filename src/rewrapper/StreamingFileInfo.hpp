// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef REWRAPPER_STREAMINGFILEINFO_HPP
#define REWRAPPER_STREAMINGFILEINFO_HPP

#include <string>
#include <boost/optional.hpp>
#include <utility>
#include "ContentRange.hpp"

struct ResumeInfo
{
  ContentRange contentRange;
  std::string sessionID;
};

struct StreamingFileInfo
{
  StreamingFileInfo(
    std::string filename,
    boost::optional<std::uint64_t> contentLength,
    boost::optional<ResumeInfo>  resumeInfo,
    boost::optional<std::string>  destinationSpecifier) :
      filename(std::move(filename)),
      contentLength(std::move(contentLength)),
      resumeInfo(std::move(resumeInfo)),
      destinationSpecifier(std::move(destinationSpecifier)) {}

  StreamingFileInfo() = default;

  std::string filename;
  boost::optional<std::uint64_t> contentLength;
  boost::optional<ResumeInfo> resumeInfo;
  boost::optional<std::string> destinationSpecifier;
};


#endif //REWRAPPER_STREAMINGFILEINFO_HPP
