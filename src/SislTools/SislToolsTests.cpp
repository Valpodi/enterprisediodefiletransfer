// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "SislTools.hpp"
#include <catch.hpp>

TEST_CASE("RapidJsonSislConverterInterface Strings Json to Sisl")
{
  REQUIRE(SislTools::toSisl("{}") == "{}");
  REQUIRE(SislTools::toSisl("{\"name\": \"donald\"}") == "{name: !str \"donald\"}");
}

TEST_CASE("RapidJsonSislConverterInterface Strings Sisl to Json")
{
  REQUIRE(SislTools::toJson("{}") == "{}");
  REQUIRE(SislTools::toJson("{name: !str \"donald\"}") == "{\"name\":\"donald\"}");
}

TEST_CASE("RapidJsonSislConverterInterface Errors Sisl Exception")
{
  REQUIRE_THROWS_AS(SislTools::toSisl("{"), UnableToParseJsonException);
}

TEST_CASE("RapidJsonSislConverterInterface Errors Json Exception")
{
  REQUIRE_THROWS_AS(SislTools::toJson("{"), UnableToParseSislException);
}

TEST_CASE("RapidJsonSislConverterInterface Errors Empty Sisl")
{
  REQUIRE_THROWS_AS(SislTools::toJson(""), UnableToParseSislException);
}

TEST_CASE("RapidJsonSislConverterInterface Errors Empty Json")
{
  REQUIRE_THROWS_AS(SislTools::toSisl(""), UnableToParseJsonException);
}
