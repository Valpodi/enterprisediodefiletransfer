// Copyright PA Knowledge 2021

#include "SISLFilename.hpp"
#include <SislTools/SislTools.hpp>
#include <iostream>
#include <rapidjson/document.h>
#include <regex>

SISLFilename::SISLFilename(std::uint32_t maxSislLength, std::uint32_t maxFilenameLength):
    maxSislLength(maxSislLength),
    maxFilenameLength(maxFilenameLength)
{}

std::optional<std::string> SISLFilename::extractFilename(const BytesBuffer& eofFrame) const
{
  const auto sislHeader = std::string(eofFrame.begin(), eofFrame.end());

  if (sislHeader.size() > maxSislLength)
  {
    std::cerr << "SISL too long" << "\n";
    return std::optional<std::string>();
  }

  try
  {
    const auto filename = convertFromSisl(sislHeader);
    if (filename.size() > maxFilenameLength)
    {
      std::cerr << "Filename too long" << "\n";
      return std::optional<std::string>();
    }
    return std::regex_match(filename, filter) ? filename : std::optional<std::string>();
  }
  catch (UnableToParseSislException& ex)
  {
    std::cerr << "Unable to parse SISL filename as SISL: " << ex.what() << "\n";
    return std::optional<std::string>();
  }
  catch (std::regex_error& ex)
  {
    std::cerr << "Failed filename sanity checks: " << ex.what() << "\n";
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
