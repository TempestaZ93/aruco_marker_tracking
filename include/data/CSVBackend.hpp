#include "data/ADataBackend.hpp"

#pragma once

namespace amt {

class CSVBackend : public ADataBackend {
   public:
    CSVBackend(std::string filename) : ADataBackend(filename) {}
    ~CSVBackend() override = default;

    std::string get_marker_info(int32_t id) override;
    std::vector<int32_t> get_data(int32_t id) override;

    int32_t connect() override;
};

}  // namespace amt
