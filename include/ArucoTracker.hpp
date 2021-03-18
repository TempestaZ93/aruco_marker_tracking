#include <opencv2/aruco.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

// maybe replace with more performant map implementation for faster lookup.
// Lookup should not occur too often, though.
#include <map>
#include <memory>
#include <tuple>
#include <vector>
// may be possible to replace with default C fn pointer
#include <functional>

#include "AMarker.hpp"
#include "data/ADataBackend.hpp"
#include "data/DataAccess.hpp"

#pragma once

namespace amt {

using Callback_t = std::function<void(int32_t)>;
using Board_t = cv::aruco::Board;

class ArucoTracker {
   public:
    ArucoTracker(std::vector<std::shared_ptr<AMarker>> markers,
                 float marker_size, std::string camera_calib_file);

    void add_marker(std::shared_ptr<AMarker> marker);
    void remove_marker(int32_t id);
    void update(cv::Mat image);

    void get_markers(std::vector<cv::Mat>& markers_out, int size);

   private:
    auto marker_by_id(int32_t id);

   private:
    // all currently tracked marker ids for faster access
    std::vector<int32_t> m_marker_ids;
    // vector containing all markers
    std::vector<std::shared_ptr<AMarker>> m_markers;
    // size of markers in meters
    float m_marker_size;
    // list of marker ids tracked last frame to track changes
    std::vector<int> m_last_tracked_ids;
    // dictionary used by opencv to get markers information
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    // aruco parameters for tracking line threshold and marker variance, border
    // deadzone and so on
    cv::Ptr<cv::aruco::DetectorParameters> m_parameters;

    // Camera calibration variables
    cv::Mat m_camera_matrix;
    cv::Mat m_dist_coeffs;
};

}  // namespace amt

