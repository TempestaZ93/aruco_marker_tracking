#include <opencv2/highgui.hpp>

#include "AMarker.hpp"

#pragma once

namespace amt {
class CubeMarker : public AMarker {
   public:
    CubeMarker(int id, std::string name, float cube_size);

    virtual void on_appear(cv::Vec3d rotation, cv::Vec3d translation) override;
    virtual void on_present(cv::Vec3d rotation, cv::Vec3d translation) override;
    virtual void on_disappear() override;

   private:
    float m_cube_size;
};
}  // namespace amt
