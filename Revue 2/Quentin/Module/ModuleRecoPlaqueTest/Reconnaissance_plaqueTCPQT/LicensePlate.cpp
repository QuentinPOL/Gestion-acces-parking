#include "LicensePlate.h"

// Random generator for cv::Scalar
cv::RNG rng(12345);

// Localize the possible Licenses Plates
std::vector<std::vector<cv::Point>> LicensePlate::locateCandidates(cv::Mat& frame)
{
	// Reduce the image dimension to process (for speed up)
	cv::Mat processedFrame = frame;
	cv::resize(frame, processedFrame, cv::Size(512, 512));

	// Must be converted to grayscale
	if (frame.channels() == 3)
		cv::cvtColor(processedFrame, processedFrame, cv::COLOR_BGR2GRAY);

	// Perform blackhat morphological operation
	cv::Mat blackhatFrame;
	cv::Mat rectangleKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(13, 5)); // Shapes are set 13 pixels wide by 5 pixels tall [Rectangle 13 * 5]

	// Reveal dark regions on light backgrounds
	cv::morphologyEx(processedFrame, blackhatFrame, cv::MORPH_BLACKHAT, rectangleKernel);

	// Find license plate based on whiteness property
	cv::Mat lightFrame;
	cv::Mat squareKernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
	cv::morphologyEx(processedFrame, lightFrame, cv::MORPH_CLOSE, squareKernel);
	cv::threshold(lightFrame, lightFrame, 0, 255, cv::THRESH_OTSU);

	// Compute Sobel gradient representation from blackhat using 32 float,
	cv::Mat gradX;
	double minVal, maxVal;
	int dx = 1, dy = 0, ddepth = CV_32F, ksize = -1;
	cv::Sobel(blackhatFrame, gradX, ddepth, dx, dy, ksize); // Looks coarse if imshow, because the range is high?

	// Convert it back to normal [0, 255]
	gradX = cv::abs(gradX);
	cv::minMaxLoc(gradX, &minVal, &maxVal);
	gradX = 255 * ((gradX - minVal) / (maxVal - minVal));
	gradX.convertTo(gradX, CV_8U);

	// Blur the gradient result
	cv::GaussianBlur(gradX, gradX, cv::Size(5, 5), 0);

	// Apply closing operation
	cv::morphologyEx(gradX, gradX, cv::MORPH_CLOSE, rectangleKernel);
	cv::threshold(gradX, gradX, 0, 255, cv::THRESH_OTSU);

	// Erode and dilate
	cv::erode(gradX, gradX, 2);
	cv::dilate(gradX, gradX, 2);

	// Bitwise AND between threshold result and light regions
	cv::bitwise_and(gradX, gradX, lightFrame);
	cv::dilate(gradX, gradX, 2);
	cv::erode(gradX, gradX, 1);

	// Find contours in the thresholded image
	std::vector<std::vector<cv::Point>> contours;
	cv::findContours(gradX, contours, cv::noArray(), cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

	// Sort by size
	std::sort(contours.begin(), contours.end(), compareContourAreas);

	// A voir
	std::vector<std::vector<cv::Point>> top_contours;
	if (!contours.empty()) {
		// Calculate the number of contours to keep
		int num_contours_to_keep = std::min(KEEP, static_cast<int>(contours.size()));

		// Assign the top contours to the output vector
		std::vector<std::vector<cv::Point>>::iterator start = contours.end() - num_contours_to_keep;
		top_contours.assign(start, contours.end());
	}

	return top_contours;
}

// Draw the possible Licenses Plates
void LicensePlate::drawLicensePlate(cv::Mat& frame, std::vector<std::vector<cv::Point>>& candidates)
{
	const int width = frame.cols;
	const int height = frame.rows;
	const float ratio_width = width / (float)512;    // WARNING! Aspect ratio may affect the performance (TO DO LIST)
	const float ratio_height = height / (float)512;  // WARNING! Aspect ratio may affect the performance

	// Convert to rectangle
	std::vector<cv::Rect> rectangles;
	for (std::vector<cv::Point> currentCandidate : candidates)
	{
		cv::Rect temp = cv::boundingRect(currentCandidate);
		float difference = temp.area() - cv::contourArea(currentCandidate);

		// Filter out the non - rectangle - shape.

		if (difference < RECT_DIFF)
			rectangles.push_back(temp);
	}

	// Remove rectangle with ... shape.
	rectangles.erase(std::remove_if(rectangles.begin(), rectangles.end(), [](cv::Rect temp)
		{
			const float aspect_ratio = temp.width / (float)temp.height;
	return aspect_ratio < MIN_AR || aspect_ratio > MAX_AR;
		}), rectangles.end());

	// Drawing rectangle (the possible plates)
	for (cv::Rect rectangle : rectangles)
	{
		cv::Rect roi(rectangle.x * ratio_width, rectangle.y * ratio_height, rectangle.width * ratio_width, rectangle.height * ratio_height);

		// Extraire la région d'intérêt (ROI)
		cv::Mat cropped = frame(roi);

		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::rectangle(frame, cv::Point(rectangle.x * ratio_width, rectangle.y * ratio_height), cv::Point((rectangle.x + rectangle.width) * ratio_width, (rectangle.y + rectangle.height) * ratio_height), color, 3, cv::LINE_8, 0);

		// Afficher l'image rectangulaire
		cv::imshow("Cropped Image", cropped);

		// Initialiser Tesseract
		tesseract::TessBaseAPI api;

		if (api.Init("C:\\Program Files\\Tesseract-OCR\\tessdata", "eng"))
		{
			qDebug() << "Erreur d'initialisation de Tesseract.";
			exit(0);
		}

		// Convertir l'image OpenCV à un format compréhensible par Tesseract
		api.SetImage(cropped.data, cropped.cols, cropped.rows, 3, cropped.step);

		// Extraire le texte
		char* text = api.GetUTF8Text();
		QString resultPlateNumber;

		if (text != nullptr)
		{
			// on Utilise une expression régulière pour correspondre à un numéro de plaque d'immatriculation français
			std::regex plateNumberRegex("([A-Z]{2}-[0-9]{3}-[A-Z]{2})");

			// on Converti le texte en une chaîne C++
			std::string textString(text);

			// On recherche la première occurrence du motif dans la chaîne
			std::smatch match;
			if (std::regex_search(textString, match, plateNumberRegex))
				resultPlateNumber = QString::fromStdString(match[0]); // La première correspondance est la plaque d'immatriculation

			// Réinitialiser la variable text à nullptr
			text = nullptr;
		}

		if (resultPlateNumber != "")
		{
			qDebug() << "Plaque d'immatriculation : " << resultPlateNumber; // Afficher la nouvelle chaîne composée uniquement de caractères valides
			this->licensePlate = resultPlateNumber; // Ducoup on met la plaque dedans
		}
		else
			qDebug() << "Plaque non reconnue : Format non reconnu !";
	}

	//// Debug code for drawing contours
	cv::Mat drawing = cv::Mat::zeros(frame.size(), CV_8UC3);
	std::vector<cv::Vec4i> hierarchy;

	for (int i = 0; i < candidates.size(); i++) 
	{
		cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		cv::drawContours(drawing, candidates, i, color, 2, 8, hierarchy, 0, cv::Point());
	}

	cv::imshow("Drawing", drawing);
}

QString LicensePlate::getLicensePlate()
{
	QString temp = this->licensePlate;
	this->licensePlate = "";

	return temp;
}