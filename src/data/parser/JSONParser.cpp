#include "data/parser/JSONParser.hpp"

namespace amt {

// helper functions
int get_line_number(const char* string, int char_pos, int& previous_line_break);

JSONParser::JSONParser() {}

JSONObject JSONParser::parse(std::string json_string) {
    const char* json_cstring = json_string.c_str();
    const char* begin = json_cstring;

    JSONObject object;
    int result = this->parse_object(json_cstring, object);

    if (result == json_string.length()) {
        return object;
    } else {
        int error_char = json_cstring - begin;
        int prev_lb = 0;
        int line = get_line_number(begin, error_char, prev_lb);
        std::cout << "Error on: " << line << ":" << error_char - prev_lb
                  << "\n";
    }
}

private:
int JSONParser::parse_null(const char* string, JSONNull& output) {
    static std::string value = "null";
    int consumed = 0;
    // loop while string is not terminated
    while (string[consumed] != '\n') {
        if (string[consumed] == value[consumed]) {
            consumed++;
        } else {
            break;
        }
    }

    if (consumed == value.length()) {
        return consumed;
    } else {
        string -= consumed;
        return 0;
    }
}

int JSONParser::parse_bool(const char* string, JSONBool& output) {
    static std::string value_true = "true";
    static std::string value_false = "false";

    // decide which value to use
    std::string value;
    bool variant = true;
    if (string[0] == 't') {
        value = value_true;
    } else if (string[0] == 'f') {
        value = value_false;
        variant = false;
    } else {
        return 0;
    }

    // test for the decided value
    int consumed = 0;
    while (string[consumed] != '\n') {
        if (string[consumed] == value[consumed]) {
            consumed++;
        } else {
            break;
        }
    }

    // test if decided value was found
    if (variant == true && consumed == value_true.length() ||
        variant == false && consumed == value_false.length()) {
        return consumed;
    } else {
        return 0;
    }
}
int JSONParser::parse_number(const char* string, JSONNumber& output) {}
int JSONParser::parse_string(const char* string, JSONString& output) {}
int JSONParser::parse_array(const char* string, JSONArray& output) {}
int JSONParser::parse_object(const char* string, JSONObject& output) {}

int get_line_number(const char* string, int char_pos, int& prev_lb) {
    int line_breaks = 0;

    for (int i = 0; i < char_pos; i++) {
        if (string[i] == '\n') {
            line_breaks++;
            prev_lb = i;
        }
    }

    return line_breaks;
}

}  // namespace amt
