// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#ifndef ED_SISLTOOLS_HPP
#define ED_SISLTOOLS_HPP
#include <string>

struct UnableToParseJsonException : public std::exception
{
  [[nodiscard]] const char * what () const noexcept override
  {
    return "Unable to parse json";
  }
};

struct UnableToParseSislException : public std::exception
{
  [[nodiscard]] const char * what () const noexcept override
  {
    return "Unable to parse sisl";
  }
};

namespace SislTools
{
  std::string toSisl(const std::string& json);
  std::string toJson(const std::string& sisl);
}

#endif //GATEWAY_RAPIDJSONSISLCONVERTERINTERFACE_HPP
