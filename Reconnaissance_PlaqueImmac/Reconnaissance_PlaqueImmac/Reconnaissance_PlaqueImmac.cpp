#include "LicensePlate.h"

int main(int agrc, char** argv) 
{
    /*
         __     __    __     ______     ______     ______
        /\ \   /\ "-./  \   /\  __ \   /\  ___\   /\  ___\
        \ \ \  \ \ \-./\ \  \ \  __ \  \ \ \__ \  \ \  __\
         \ \_\  \ \_\ \ \_\  \ \_\ \_\  \ \_____\  \ \_____\
          \/_/   \/_/  \/_/   \/_/\/_/   \/_____/   \/_____/
    */

    // Instantiate LicensePlate object
    LicensePlate lp;

    std::string filename = "Resources/plaque-ww.jpg";
    cv::Mat image;
    image = cv::imread(filename, cv::IMREAD_COLOR);

    if (!image.data) 
    {
        std::cout << "ERREUR: IMPOSSIBLE DE LIRE L'IMAGE DEPUIS LE FICHIER\n\n";
        return -1;
    }

    std::vector<std::vector<cv::Point>> candidates = lp.locateCandidates(image);
    lp.drawLicensePlate(image, candidates);

    imshow("Plates Detection", image);
    cv::waitKey(0);

    /*
       __   __   __     _____     ______     ______
      /\ \ / /  /\ \   /\  __-.  /\  ___\   /\  __ \
      \ \ \'/   \ \ \  \ \ \/\ \ \ \  __\   \ \ \/\ \
       \ \__|    \ \_\  \ \____-  \ \_____\  \ \_____\
        \/_/      \/_/   \/____/   \/_____/   \/_____/
    */

    /*
    cv::VideoCapture cap("demo.mp4");
    if (!cap.isOpened()) {
      std::cout << "Error opening video stream or file" << std::endl;
    }

    while (true) {
      // Image digester
      cv::Mat image;
      cap >> image;
      if (image.empty()) break;

      // Keyboard listener
      char c = (char) cv::waitKey(1);
      if (c == 200) break;

      // Processing technique
      std::vector<std::vector<cv::Point>> candidates = lp.locateCandidates(image);

      cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);
      std::vector<cv::Vec4i> hierarchy;
      for (int i = 0; i < candidates.size(); i++) {
        cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
        cv::drawContours(drawing, candidates, i, color, 2, 8, hierarchy, 0, cv::Point() );
      }
      cv::imshow("Drawing", drawing);
      cv::waitKey(0);

      lp.drawLicensePlate(image, candidates);
      lp.viewer(image, "Frame");
    }
    cap.release();
    cv::destroyAllWindows();
    */


}

/*
// Debug code for drawing contours
cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);
std::vector<cv::Vec4i> hierarchy;
for (int i = 0; i < candidates.size(); i++) {
  cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
  cv::drawContours(drawing, candidates, i, color, 2, 8, hierarchy, 0, cv::Point() );
}
cv::imshow("Drawing", drawing);
cv::waitKey(0);
*/