#include "CubeMarker.hpp"

#include <iostream>
#include <opencv2/calib3d.hpp>
#include <opencv2/opencv.hpp>

namespace amt {

std::vector<cv::Point3d> cube_points = {
    {-0.5f, -0.5f, 0.0f}, {0.5f, -0.5f, 0.0f},  {0.5f, 0.5f, 0.0f},
    {-0.5f, 0.5f, 0.0f},  {-0.5f, -0.5f, 1.0f}, {0.5f, -0.5f, 1.0f},
    {0.5f, 0.5f, 1.0f},   {-0.5f, 0.5f, 1.0f}};

std::vector<int> cube_lines = {0, 1, 1, 2, 2, 3, 3, 0, 4, 5, 5, 6,
                               6, 7, 7, 4, 0, 4, 1, 5, 2, 6, 3, 7};

std::vector<cv::Point3d> get_scaled_object_points(float cube_size) {
    std::vector<cv::Point3d> scaled_points;

    for (auto point : cube_points) {
        scaled_points.push_back(point * cube_size);
    }
    return scaled_points;
}

CubeMarker::CubeMarker(int id, std::string name, float cube_size)
    : WireframeMarker(id, name, get_scaled_object_points(cube_size),
                      cube_lines) {}

void CubeMarker::on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                           const Camera& camera, cv::Mat image) {
    std::cout << this->name << " found.\n";
}

void CubeMarker::on_present(cv::Vec3d rotation, cv::Vec3d translation,
                            const Camera& camera, cv::Mat image) {
    base_type::on_present(rotation, translation, camera, image);
}

void CubeMarker::on_disappear(cv::Mat image) {
    std::cout << this->name << " lost.\n";
}

}  // namespace amt
