// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "RapidJsonSislConverter.hpp"
#include <catch.hpp>

TEST_CASE("Json parsing. Arrays not supported")
{
  REQUIRE_THROWS_AS(SislToolsInternal::parseJsonToSisl("{ \"hello\" : [1,2,3]}"), std::runtime_error);
}

TEST_CASE("Json parsing. Nested Groups")
{
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : { \"key\": 123 }}") == "{hello: !obj {key: !uint \"123\"}}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : { \"key\": 123 }, \"hello\" : { \"key\": 123 }}") ==
    "{hello: !obj {key: !uint \"123\"},hello: !obj {key: !uint \"123\"}}");
}

TEST_CASE("Json parsing. Create appropriate sisl type")
{
  REQUIRE(SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\"}") == "{hello: !str \"world\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"name\" : \"KA\"}") ==
    "{hello: !str \"world\",name: !str \"KA\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : 1}") ==
    "{hello: !str \"world\",value: !uint \"1\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : -1}") ==
    "{hello: !str \"world\",value: !int \"-1\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : true}") ==
    "{hello: !str \"world\",value: !bool \"true\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : false}") ==
    "{hello: !str \"world\",value: !bool \"false\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : null}") ==
    "{hello: !str \"world\",value: !null \"\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{ \"hello\" : \"world\", \"value\" : 0.123412}") ==
    "{hello: !str \"world\",value: !double \"0.123412\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{\"value\" : 4294967296}") ==
    "{value: !uint64_t \"4294967296\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl("{\"value\" : -2147483649}") ==
    "{value: !int64_t \"-2147483649\"}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl(
      "{"
      "  \"_class\" : \"org.jenkinsci.plugins.workflow.job.WorkflowJob\","
      "  \"displayName\" : \"proxysettings\""
      "}") ==
    "{"
    "_class: !str \"org.jenkinsci.plugins.workflow.job.WorkflowJob\","
    "displayName: !str \"proxysettings\""
    "}");
  REQUIRE(
    SislToolsInternal::parseJsonToSisl(
      "{"
      "  \"_class\" : \"org.jenkinsci.plugins.workflow.job.WorkflowJob\","
      "  \"buildable\" : false"
      "}") == "{"
              "_class: !str \"org.jenkinsci.plugins.workflow.job.WorkflowJob\","
              "buildable: !bool \"false\""
              "}");
}
