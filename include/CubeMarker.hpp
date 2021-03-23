#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "WireframeMarker.hpp"

#pragma once

namespace amt {
class CubeMarker : public WireframeMarker {
    using base_type = WireframeMarker;

   public:
    CubeMarker(int id, std::string name, float cube_size);

    virtual void on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                           const Camera& camera, cv::Mat image) override;
    virtual void on_present(cv::Vec3d rotation, cv::Vec3d translation,
                            const Camera& camera, cv::Mat image) override;
    virtual void on_disappear(cv::Mat image) override;
};
}  // namespace amt
