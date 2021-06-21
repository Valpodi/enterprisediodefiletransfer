// Copyright PA Knowledge 2021

#include "SISLFilename.hpp"
#include <SislTools/SislTools.hpp>
#include <iostream>
#include <rapidjson/document.h>
#include "spdlog/spdlog.h"

SISLFilename::SISLFilename(std::uint32_t maxSislLength, std::uint32_t maxFilenameLength):
    maxSislLength(maxSislLength),
    maxFilenameLength(maxFilenameLength)
{}

std::optional<std::string> SISLFilename::extractFilename(const BytesBuffer& eofFrame) const
{
  const auto sislHeader = std::string(eofFrame.begin(), eofFrame.end());

  if (sislHeader.size() > maxSislLength || sislHeader.size() < 2)
  {
    spdlog::error("SISL too long/short");
    return std::optional<std::string>();
  }

  if (sislHeader.at(0) != '{')
  {
    spdlog::error("EOF not SISL");
    return std::optional<std::string>();
  }

  try
  {
    const auto filename = convertFromSisl(sislHeader);
    if (filename.size() > maxFilenameLength)
    {
      spdlog::error("Filename too long");
      return std::optional<std::string>();
    }
    return std::regex_match(filename, filter) ? filename : std::optional<std::string>();
  }
  catch (UnableToParseSislException& ex)
  {
    spdlog::error(std::string("Unable to parse SISL filename as SISL: ") + ex.what());
    return std::optional<std::string>();
  }
  catch (std::regex_error& ex)
  {
    spdlog::error(std::string("Failed filename sanity checks: ") + ex.what());
    return std::optional<std::string>();
  }

}

std::string SISLFilename::convertFromSisl(const std::string& sislFrame)
{
  const auto doc = parseSisl(sislFrame);
  return doc.HasMember("name") ? doc["name"].GetString() : "";
}

rapidjson::Document SISLFilename::parseSisl(const std::string& sislFrame)
{
  rapidjson::Document doc;
  doc.Parse(SislTools::toJson(sislFrame).c_str());
  return doc;
}
