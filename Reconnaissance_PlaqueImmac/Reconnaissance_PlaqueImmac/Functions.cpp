#include "Functions.h"

bool compareContourAreas(std::vector<cv::Point>& contour1, std::vector<cv::Point>& contour2) 
{
    const double i = fabs(contourArea(cv::Mat(contour1)));
    const double j = fabs(contourArea(cv::Mat(contour2)));

    return (i < j);
}