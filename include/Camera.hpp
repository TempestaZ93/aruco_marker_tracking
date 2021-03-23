#include <opencv2/core.hpp>

#pragma once

namespace amt {

struct Camera {
    cv::Mat camera_matrix;
    cv::Mat distortion_coeffs;
};

}  // namespace amt
