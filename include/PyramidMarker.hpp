#include "WireframeMarker.hpp"

#pragma once

namespace amt {

class PyramidMarker : public WireframeMarker {
    using base_type = WireframeMarker;

   public:
    PyramidMarker(int id, std::string name, float base_scaling, float height);

    // function gets called once when the marker is detected
    virtual void on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                           const Camera& camera, cv::Mat image) override;

    // function gets called repeadetly while the marker is visible
    virtual void on_present(cv::Vec3d rotation, cv::Vec3d translation,
                            const Camera& camera, cv::Mat image) override;

    // function gets called once when the marker is no longer detected
    virtual void on_disappear(cv::Mat image) override;
};

}  // namespace amt
