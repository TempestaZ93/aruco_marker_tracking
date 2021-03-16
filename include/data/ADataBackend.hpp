#include <string>
#include <vector>

#pragma once
namespace amt {
class ADataBackend {
   public:
    ADataBackend(std::string name) : m_name(name) {}
    virtual ~ADataBackend() = default;

    virtual std::string get_marker_info(int32_t id) = 0;
    virtual std::vector<int32_t> get_data(int32_t id) = 0;

    virtual int32_t connect() = 0;

   protected:
    std::string m_name;
};
}  // namespace amt
