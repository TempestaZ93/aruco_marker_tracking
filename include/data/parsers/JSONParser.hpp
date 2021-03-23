#include <sstream>
#include <string>

#include "data/parser/json/JSONObject.hpp"

#pragma once

static std::string grammar = R"(#!grammar:json
object_begin={
object_end=}
array_begin=[
array_end=]
string_begin="
string_end="
object_separator=,
assignment_operator=:
decimal_separator=.
)";

namespace amt {

class JSONParser {
   public:
    JSONParser();

    JSONObject parse(std::string json_string);

   private:
    int parse_null(const char* string, JSONNull& output);
    int parse_bool(const char* string, JSONBool& output);
    int parse_number(const char* string, JSONNumber& output);
    int parse_string(const char* string, JSONString& output);
    int parse_array(const char* string, JSONArray& output);
    int parse_object(const char* string, JSONObject& output);
};

}  // namespace amt
