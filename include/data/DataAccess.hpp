#include <memory>

#include "data/ADataBackend.hpp"

#pragma once

namespace amt {

class DataAccess {
   public:
    DataAccess(std::shared_ptr<ADataBackend> backend) {}

   private:
};

}  // namespace amt

