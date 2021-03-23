#include "WireframeMarker.hpp"

#include <opencv2/calib3d.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
namespace amt {

WireframeMarker::WireframeMarker(int id, std::string name,
                                 std::vector<cv::Point3d> object_points,
                                 std::vector<int> line_pairs)
    : AMarker(id, name),
      m_object_points(object_points),
      m_line_pairs(line_pairs) {}

// function gets called once when the marker is detected
void WireframeMarker::on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                                const Camera& camera, cv::Mat image) {}

// function gets called repeadetly while the marker is visible
void WireframeMarker::on_present(cv::Vec3d rotation, cv::Vec3d translation,
                                 const Camera& camera, cv::Mat image) {
    std::vector<cv::Point2d> image_points;

    cv::projectPoints(this->m_object_points, rotation, translation,
                      camera.camera_matrix, camera.distortion_coeffs,
                      image_points);

    for (int line = 0; line < this->m_line_pairs.size() / 2; line++) {
        int start = line * 2;
        int end = line * 2 + 1;
        cv::line(image, image_points[this->m_line_pairs[start]],
                 image_points[this->m_line_pairs[end]], cv::Scalar(255, 0, 0),
                 2, cv::LINE_8);
    }
}

// function gets called once when the marker is no longer detected
void WireframeMarker::on_disappear(cv::Mat image) {}

}  // namespace amt
