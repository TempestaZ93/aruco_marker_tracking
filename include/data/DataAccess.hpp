#include <memory>

#include "data/ADataBackend.hpp"

#pragma once

namespace amt {

class DataAccess {
   public:
    DataAccess(std::shared_ptr<ADataBackend> backend) : m_backend(backend) {}

   private:

    std::shared_ptr<ADataBackend> m_backend;
};

}  // namespace amt

