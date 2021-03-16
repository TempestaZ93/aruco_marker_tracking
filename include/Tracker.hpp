#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

#pragma once

namespace amt {
class Tracker {
public:
  Tracker();

  cv::Mat get_dictionary();

private:
  cv::Ptr<cv::aruco::Dictionary> m_aruco_dictionary;
};
}
