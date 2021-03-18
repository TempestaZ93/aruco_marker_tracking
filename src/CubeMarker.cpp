#include "CubeMarker.hpp"

#include <iostream>

namespace amt {

CubeMarker::CubeMarker(int id, std::string name, float cube_size)
    : AMarker(id, name), m_cube_size(cube_size) {}

void CubeMarker::on_appear(cv::Vec3d rotation, cv::Vec3d translation) {
    std::cout << "Found Cube Marker\n";
}

void CubeMarker::on_present(cv::Vec3d rotation, cv::Vec3d translation) {
    std::cout << "See Cube Marker\n";
}

void CubeMarker::on_disappear() { std::cout << "Lost Cube Marker\n"; }

}  // namespace amt
