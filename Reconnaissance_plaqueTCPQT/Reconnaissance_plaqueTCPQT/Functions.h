#pragma once

#include "opencv2/opencv.hpp"
#include <vector>

bool compareContourAreas(std::vector<cv::Point>& contour1, std::vector<cv::Point>& contour2);