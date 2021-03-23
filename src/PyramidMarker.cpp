#include "PyramidMarker.hpp"

#include <iostream>
namespace amt {
std::vector<cv::Point3d> pyramid_points = {
    {-0.5, -0.5, 0}, {0.5, -0.5, 0}, {0.5, 0.5, 0}, {-0.5, 0.5, 0}, {0, 0, 1}};
std::vector<int> pyramid_lines = {0, 1, 1, 2, 2, 3, 0, 4, 1, 4, 2, 4, 3, 4};

std::vector<cv::Point3d> get_scaled_object_points(float base_scaling,
                                                  float height) {
    std::vector<cv::Point3d> scaled_points;

    for (auto point : pyramid_points) {
        scaled_points.push_back(point * base_scaling);
    }

    scaled_points[4] *= height;

    return scaled_points;
}

PyramidMarker::PyramidMarker(int id, std::string name, float base_scaling,
                             float height)
    : WireframeMarker(id, name, get_scaled_object_points(base_scaling, height),
                      pyramid_lines) {}

// function gets called once when the marker is detected
void PyramidMarker::on_appear(cv::Vec3d rotation, cv::Vec3d translation,
                              const Camera& camera, cv::Mat image) {
    std::cout << this->name << " found.\n";
}

// function gets called repeadetly while the marker is visible
void PyramidMarker::on_present(cv::Vec3d rotation, cv::Vec3d translation,
                               const Camera& camera, cv::Mat image) {
    base_type::on_present(rotation, translation, camera, image);
}

// function gets called once when the marker is no longer detected
void PyramidMarker::on_disappear(cv::Mat image) {
    std::cout << this->name << " lost.\n";
}

}  // namespace amt

