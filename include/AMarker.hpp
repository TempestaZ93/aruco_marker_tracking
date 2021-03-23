#include <opencv2/core.hpp>
#include <string>

#include "Camera.hpp"

#pragma once

namespace amt {
// marker class representing a tracked marker. It is used to handle events
// regarding the marker states: APPEARED, PRESENT, DISAPPEARED
class AMarker {
   public:
    AMarker(int32_t id, std::string name) : id(id), name(name) {}
    virtual ~AMarker() = default;

    // return aruco marker id
    int32_t get_id();

    // return name of marker
    std::string get_name();

    // function gets called once when the marker is detected
    virtual void on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                           const Camera& camera, cv::Mat image) = 0;

    // function gets called repeadetly while the marker is visible
    virtual void on_present(cv::Vec3d rotation, cv::Vec3d translation,
                            const Camera& camera, cv::Mat image) = 0;

    // function gets called once when the marker is no longer detected
    virtual void on_disappear(cv::Mat image) = 0;

   protected:
    // Marker id
    int32_t id;
    // arbitrary name for the marker
    std::string name;
};
}  // namespace amt
