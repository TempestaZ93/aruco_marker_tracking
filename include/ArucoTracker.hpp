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
#include <type_traits>

#include "data/ADataBackend.hpp"
#include "data/DataAccess.hpp"

#pragma once

namespace amt {

using Callback_t = std::function<void(int32_t)>;
using Board_t = cv::aruco::Board;

class ArucoTracker {
   public:
    ArucoTracker(const std::vector<int32_t>& ids,
                 const std::vector<Callback_t>& callbacks,
                 float real_size,
                 std::string camera_calib_file,
                 std::shared_ptr<ADataBackend> backend);

    void set_callback(int32_t id, Callback_t callback);
    void remove_callback(int32_t id);
    void update(cv::Mat image);

    void get_markers(std::vector<cv::Mat>& markers_out, int size);

   private:
    std::vector<int32_t> m_marker_ids;
    // map containing all marker ids with the corrosponding callbacks
    std::vector<Callback_t> m_callbacks;
    // Real size of markers (only one measurement required, since they are square
    float m_real_size;
    // access to data bases for retreiving whatever from information to markers_out
    std::shared_ptr<DataAccess> m_data_access;
    // dictionary used by opencv to get markers information
    cv::Ptr<cv::aruco::Dictionary> m_dictionary;
    // aruco parameters for tracking line threshold and marker variance, border deadzone and so on
    cv::Ptr<cv::aruco::DetectorParameters> m_parameters;
    // moving median for translation and rotation
    uint64_t m_moving_median_length;
    std::vector<cv::Mat> m_moving_median_rotation;
    std::vector<cv::Mat> m_moving_median_translation;


    // Camera calibration variables
    cv::Mat m_camera_matrix;
    cv::Mat m_dist_coeffs;
};

}  // namespace amt

