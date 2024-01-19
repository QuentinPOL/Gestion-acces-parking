#include "LicensePlate.h"
#include <conio.h>

int main(int agrc, char** argv) 
{
    /*
    ////////// ON A L'IMAGE ORGINIAL //////////

    // Charger l'image depuis un fichier et l'ouvrir
    Mat imageOriginal = imread("Resources/car3_W5avzVC0Sx.jpg");

    ////////// ON FAIT LE PREPROCESS //////////

    if (imageOriginal.empty()) 
    {
        cout << "ERREUR: IMPOSSIBLE DE LIRE L'IMAGE DEPUIS LE FICHIER\n\n";     
        _getch(); // En attente d'une touche pour fermer le programme                                              
        return(0);                                              
    }

    // Convertir l'image en niveaux de gris
    Mat gray; // matrice qui définis une image
    cvtColor(imageOriginal, gray, COLOR_BGR2GRAY); // Mettre l'image en Gris

    // Appliquer un lissage pour réduire le bruit (les pixels qui servent à rien)
    GaussianBlur(gray, gray, Size(3, 3), 0, 1);

    // Appliquer la détection des bords avec Canny
    Mat edges; // matrice qui définis une image
    Canny(gray, edges, 200, 100); // On utilise l'algo canny pour trouver les "edges" dans une image

    // Trouver les contours dans l'image
    vector<vector<Point>> contoursIMG; // Tableaux à double dimensions qui va constituer tout les points (contours) -> x, y
    vector<Vec4i> hierachy;

    findContours(edges, contoursIMG, hierachy, RETR_LIST, CHAIN_APPROX_SIMPLE); // On va chercher à  trouver tout les contours par rapport à l'image edges qu'on à  modifier
    //drawContours(image, contoursIMG, -1, Scalar(255, 0, 0), 2);

    for (int i = 0; i < contoursIMG.size(); i++)
    {
        int area = contourArea(contoursIMG[i]); // on calcule la taille de zone à  chaque contour
        vector<vector<Point>> conPoly(contoursIMG.size()); // Tableaux à double dimensions qui va constituer tout les nouveaux points des contours -> x, y

        if (area > 1000) // On va enlever les petits contours 
        {
            float perimeter = arcLength(contoursIMG[i], true); // Ensuite on calcul le perimetre
            float epsilon = 0.01 * perimeter; // Au cas ou c'est pas une forme exact on fait une approximation

            approxPolyDP(contoursIMG[i], conPoly[i], epsilon, true); // On détermie une forme approximative avec les nouveaux points dans conPoly

            cout << conPoly[i].size() << endl;

            if (conPoly[i].size() == 4) // et si c'est un carré ou un rectangle bah  on affiche ces contours
                drawContours(imageOriginal, contoursIMG, i, Scalar(255, 0, 255), 2);
        }
    }

    // Afficher l'image résultante
    imshow("Plates Detection", imageOriginal);

    // En attente infini
    waitKey(0);

    return 0;
    */

    /*
         __     __    __     ______     ______     ______
        /\ \   /\ "-./  \   /\  __ \   /\  ___\   /\  ___\
        \ \ \  \ \ \-./\ \  \ \  __ \  \ \ \__ \  \ \  __\
         \ \_\  \ \_\ \ \_\  \ \_\ \_\  \ \_____\  \ \_____\
          \/_/   \/_/  \/_/   \/_/\/_/   \/_____/   \/_____/
    */

    // Instantiate LicensePlate object
    LicensePlate lp;

    std::string filename = "Resources/plaques-WW.jpg";
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