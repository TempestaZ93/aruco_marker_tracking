
#include "AMarker.hpp"

#pragma once
namespace amt {
class WireframeMarker : public AMarker {
   public:
    WireframeMarker(int id, std::string name, std::vector<cv::Point3d> points,
                    std::vector<int> line_pairs);

    // function gets called once when the marker is detected
    virtual void on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                           const Camera& camera, cv::Mat image) override;

    // function gets called repeadetly while the marker is visible
    virtual void on_present(cv::Vec3d rotation, cv::Vec3d translation,
                            const Camera& camera, cv::Mat image) override;

    // function gets called once when the marker is no longer detected
    virtual void on_disappear(cv::Mat image) override;

   private:
    std::vector<cv::Point3d> m_object_points;
    std::vector<int> m_line_pairs;
};
}  // namespace amt
