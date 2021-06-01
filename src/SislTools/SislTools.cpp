// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "SislTools.hpp"
#include "library/BoostSpiritSislParser.hpp"
#include "library/RapidJsonSislConverter.hpp"
#include <stdexcept>

namespace SislTools
{
  std::string toSisl(const std::string& json)
  {
    try
    {
      return SislToolsInternal::parseJsonToSisl(json);
    }
    catch (const std::runtime_error& e)
    {
      throw UnableToParseJsonException();
    }
  }
  std::string toJson(const std::string& sisl)
  {
    try
    {
      return SislToolsInternal::parseSislToJson(sisl);
    }
    catch (const std::runtime_error& e)
    {
      throw UnableToParseSislException();
    }

  }
}
