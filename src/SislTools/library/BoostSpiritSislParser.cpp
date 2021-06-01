// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include <boost/spirit/home/x3.hpp>
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"

#include <map>
#include <functional>

#include "BoostSpiritSislParser.hpp"

struct Storage
{
  Storage():
    writer(s) {}

  rapidjson::StringBuffer s;
  rapidjson::Writer<rapidjson::StringBuffer> writer;

  std::string rType;
};

namespace actions
{
  using boost::spirit::x3::_attr;

  struct state_tag
  {
  };

  //actions
  auto keyAction = [](auto& ctx) {
    boost::spirit::x3::get<state_tag>(ctx).get().writer.Key(_attr(ctx).c_str());
  };

  auto typeAction = [](auto& ctx) { boost::spirit::x3::get<state_tag>(ctx).get().rType = _attr(ctx); };

  auto valueAction = [](auto& ctx) {
    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "str")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.String(_attr(ctx).c_str());
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "bool")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Bool(std::string(_attr(ctx).c_str()) == std::string("true"));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "uint")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Uint(static_cast<unsigned int>(std::stoul(_attr(ctx).c_str())));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "int")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Int(static_cast<int>(std::stoul(_attr(ctx).c_str())));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "uint64_t")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Uint64(static_cast<uint64_t>(std::stoull(_attr(ctx).c_str())));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "int64_t")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Int64(static_cast<int64_t>(std::stoull(_attr(ctx).c_str())));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "double")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Double(static_cast<double>(std::stod(_attr(ctx).c_str())));
    }

    if (boost::spirit::x3::get<state_tag>(ctx).get().rType == "null")
    {
      boost::spirit::x3::get<state_tag>(ctx).get().writer.Null();
    }
  };

  auto startObjectAction = [](auto& ctx) {
    boost::spirit::x3::get<state_tag>(ctx).get().writer.StartObject();
  };

  auto endObjectAction = [](auto& ctx) {
    boost::spirit::x3::get<state_tag>(ctx).get().writer.EndObject();
  };
}

namespace parser
{
  using boost::spirit::x3::char_;
  using boost::spirit::x3::lexeme;
  using boost::spirit::x3::lit;

  //element syntax
  auto const name = lexeme[+(char_ - ':' - ',') >> ':'][actions::keyAction];
  auto const type = lexeme['!' >> +(char_ - ' ') >> ' '][actions::typeAction];
  auto const quoted_string = lexeme['"' >> *(char_ - '"') >> '"'][actions::valueAction];

  //rules
  boost::spirit::x3::rule<class sisl> sisl = "sisl";
  boost::spirit::x3::rule<class sisl_value> sisl_value = "sisl_value";

  auto const sisl_value_def = quoted_string | sisl;
  auto const grouping = name >> type >> sisl_value;
  auto const sisl_def = lit('{')[actions::startObjectAction] >> *(grouping % ',') >> lit('}')[actions::endObjectAction];

  BOOST_SPIRIT_DEFINE(sisl_value, sisl)
}

std::string SislToolsInternal::parseSislToJson(std::string input)
{
  Storage storage;

  auto parser = boost::spirit::x3::with<actions::state_tag>(std::ref(storage))[parser::sisl] >> boost::spirit::x3::eoi;

  using boost::spirit::x3::ascii::space;
  if (phrase_parse(input.begin(), input.end(), parser, space))
  {
    return storage.s.GetString();
  }
  throw std::runtime_error("unable to convert to json");
}
