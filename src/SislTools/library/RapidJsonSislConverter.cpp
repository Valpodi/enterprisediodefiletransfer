// Copyright PA Knowledge Ltd 2021
// MIT License. For licence terms see LICENCE.md file.

#include "rapidjson/reader.h"
#include <sstream>
#include <iostream>
#include "RapidJsonSislConverter.hpp"

using namespace rapidjson;
using namespace std;

struct MyHandler : public BaseReaderHandler<UTF8<>, MyHandler> {

  explicit MyHandler(std::stringstream& stream):
    stream(stream), nesting_depth(0) {}

  std::stringstream& stream;
  std::uint64_t nesting_depth;

  bool Null() { stream << "!null \"\","; return true; }
  bool Bool(bool b) { stream << "!bool \"" << boolalpha << b << "\","; return true; }
  bool Int(int i) { stream << "!int \"" << i << "\","; return true; }
  bool Uint(unsigned u) { stream << "!uint \"" << u << "\","; return true; }
  bool Int64(int64_t i) { stream << "!int64_t \"" << i << "\","; return true; }
  bool Uint64(uint64_t u) { stream << "!uint64_t \"" << u << "\","; return true; }
  bool Double(double d) { stream << "!double \"" << d << "\","; return true; }
  bool String(const char* str, SizeType, bool) { stream << "!str \"" << str << "\"," ; return true;}
  bool StartObject() { if (nesting_depth++ == 0) {stream << '{';} else {stream << "!obj {";} return true; }
  bool Key(const char* str, SizeType, bool) { stream << str << ": "; return true;}
  bool EndObject(SizeType memberCount) {
    if (memberCount > 0) stream.seekp(-1, std::ios_base::end);
    stream << "}";
    nesting_depth--;
    if (nesting_depth > 0) {
      stream << ",";
    }
    return true;
  }
  bool StartArray() { return false; }
  bool EndArray(SizeType elementCount) { cout << "EndArray(" << elementCount << ")" << endl; return true; }
};

std::string SislToolsInternal::parseJsonToSisl(std::string json)
{
  std::stringstream output;
  MyHandler handler(output);
  Reader reader;
  StringStream ss(json.data());
  if (!reader.Parse(ss, handler)) throw std::runtime_error("unable to parse");
  return output.str();
}
