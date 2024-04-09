#pragma once

/// OPENCV2 ///
#include "opencv2/opencv.hpp"
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

/// Tesseract OCR ////
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>

/// OTHER INCLUDE ///
#include "Functions.h"

/// LIBRAIRIES ///
#include <iostream>
#include <vector>
#include <regex>
#include <QString>
#include <QDebug>

/// GLOBAL VARIABLES ///
#define KEEP 5          // Limit the number of license plates
#define MIN_AR 1        // Minimum aspect ratio
#define MAX_AR 6        // Maximum aspect ratio
#define RECT_DIFF 2000  // Set the difference between contour and rectangle

/// CLASS TO FIND THE LICENSE PLATE ///
class LicensePlate
{
private:
    QString licensePlate;
public:
    std::vector<std::vector<cv::Point>> locateCandidates(cv::Mat& frame);
    void drawLicensePlate(cv::Mat& frame, std::vector<std::vector<cv::Point>>& candidates);

    // Accesseur
    QString getLicensePlate();
};
