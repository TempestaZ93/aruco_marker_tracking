#include <opencv2/core.hpp>
#include <iostream>

int main(int argc, char* argv[]) {
  cv::Mat mat = cv::Mat::zeros(3, 3, CV_64F);
  std::cout << "Hello OpenCV: " << std::endl << mat << std::endl;
  
  for(int i = 0; i < argc; i++) {
    std::cout << argv[i] << std::endl;
  }

  return 0;
}
