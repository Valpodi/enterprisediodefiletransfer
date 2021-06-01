// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "BoostSpiritSislParser.hpp"
#include <catch.hpp>

TEST_CASE("SISL parsing. Can parse strings")
{
  REQUIRE(SislToolsInternal::parseSislToJson("{ field: !str \"h\"}") == "{\"field\":\"h\"}");
}

TEST_CASE("SISL parsing. Can parse doubles")
{
  REQUIRE(
    SislToolsInternal::parseSislToJson("{hello: !str \"world\",value: !double \"0.123412\"}") ==
          "{\"hello\":\"world\",\"value\":0.123412}");
}

TEST_CASE("SISL parsing. Can parse booleans")
{
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !bool \"false\"}") == "{\"value\":false}");
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !bool \"true\"}") == "{\"value\":true}");
}

TEST_CASE("SISL parsing. Can parse lists")
{
  REQUIRE(
    SislToolsInternal::parseSislToJson("{ field: !str \"h\", field2: !uint \"2\"}") == "{\"field\":\"h\",\"field2\":2}");
}

TEST_CASE("SISL parsing. Can parse objs")
{
  REQUIRE(SislToolsInternal::parseSislToJson("{hello: !obj {key: !uint \"123\"}}") == "{\"hello\":{\"key\":123}}");
}

TEST_CASE("SISL parsing. Can parse lists with objects")
{
  REQUIRE(
    SislToolsInternal::parseSislToJson(R"({name: !obj {key: !str "value"}, key: !str "value"})")
          == R"({"name":{"key":"value"},"key":"value"})");
  REQUIRE(
    SislToolsInternal::parseSislToJson(R"({name: !obj {key: !str "value"}, another: !obj {key: !str "value"}})")
          == R"({"name":{"key":"value"},"another":{"key":"value"}})");
}

TEST_CASE("SISL parsing. Can parse nested lists with objects")
{
  REQUIRE(
    SislToolsInternal::parseSislToJson(
      R"({name: !obj {another: !obj {key: !str "value", dif: !uint "1"}, friend: !bool "true"}, key: !str "value"})")
          == R"({"name":{"another":{"key":"value","dif":1},"friend":true},"key":"value"})");
}

TEST_CASE("SISL parsing. Can parse empty SISL")
{
  REQUIRE(SislToolsInternal::parseSislToJson("{}") == "{}");
}

TEST_CASE("SISL parsing. Can parse empty values")
{
  REQUIRE(SislToolsInternal::parseSislToJson(R"({name: !str ""})")
  == R"({"name":""})");
}

TEST_CASE("SISL parsing. Can parse null types")
{
  REQUIRE(SislToolsInternal::parseSislToJson(R"({name: !null ""})")
          == R"({"name":null})");
}

TEST_CASE("SISL parsing. Can parse numerical types")
{
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !uint \"123\"}") == "{\"value\":123}");
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !int \"-1\"}") == "{\"value\":-1}");
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !uint64_t \"4294967296\"}") == "{\"value\":4294967296}");
  REQUIRE(SislToolsInternal::parseSislToJson("{value: !int64_t \"-2147483649\"}") =="{\"value\":-2147483649}");
}
