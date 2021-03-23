#include "data/CSVBackend.hpp"

namespace amt {
std::string CSVBackend::get_marker_info(int32_t id) { return ""; }

std::vector<int32_t> CSVBackend::get_data(int32_t id) { return {0}; }

int32_t CSVBackend::connect() { return 0; }
}  // namespace amt
