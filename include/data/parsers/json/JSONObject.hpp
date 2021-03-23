#include <vector>

#include "data/parser/json/JSONValue.hpp"

#pragma once

namespace amt {

class JSONObject : public JSONValue<JSONValueType::OBJECT> {
   public:
    JSONObject(std::vector<JSONValue> values);

    inline JSONValue& get(uint64_t index) { return m_values[index]; }
    inline const JSONValue& get(uint64_t index) const {
        return m_values[index];
    }
    JSONValue& operator[](uint64_t index) { return get(index); }
    const JSONValue& operator[](uint64_t index) const { return get(index); }

   private:
    std::vector<JSONValue> m_values;
};

}  // namespace amt
