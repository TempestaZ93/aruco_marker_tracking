#include "data/parser/json/JSONValueType.hpp"

#pragma once

namespace amt {

template <JSONValueType T>
class JSONValue {
   public:
    JSONValue() = default;
    virtual ~JSONValue() = default;
};

}  // namespace amt
