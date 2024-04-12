#include "ClientTCPIP.h"

// Constructeur
ClientTCPIP::ClientTCPIP(QObject* parent) : QObject(parent)
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Constructeur ClientTCPIP::ClientTCPIP(QObject* parent) : CALL";

	if (lp = new LicensePlate()) // Instantiate LicensePlate object
		qDebug() << "Constructeur LicensePlate : PASS";

	// On va lancer le processus d'analyse de plaque [NE PAS OUBLIER DE FAIRE UNE CLASSE CAMERA]
		/*
		 __     __    __     ______     ______     ______
		\ \   /\ "-./  \   /\  __ \   /\  ___\   /\  ___\
		\ \ \  \ \ \-./\ \  \ \  __ \  \ \ \__ \  \ \  __\
		 \ \_\  \ \_\ \ \_\  \ \_\ \_\  \ \_____\  \ \_____\
		  \/_/   \/_/  \/_/   \/_/\/_/   \/_____/   \/_____/
		*/

	std::string filename = "Resources/plaques-WW.jpg"; // A remplacer les frames de la caméra
	cv::Mat image;
	image = cv::imread(filename, cv::IMREAD_COLOR);

	if (!image.data)
		qDebug() << "ERREUR: IMPOSSIBLE DE LIRE L'IMAGE DEPUIS LE FICHIER";
	else
		qDebug() << "IMAGE CHARGER AVEC SUCCES";

	std::vector<std::vector<cv::Point>> candidates = lp->locateCandidates(image);

	if (candidates.size() != 0)
		qDebug() << "Une plaque ou plusieurs ont etaient trouves";

	lp->drawLicensePlate(image, candidates);

	// Et ensuite la retouner afin de la renvoyer au superviseur/piloteur
	// Créer un nouvel objet JSON avec la structure souhaitée
	QString Plaque = lp->getLicensePlate();
	qDebug() << "La plaque d'immatriculaton est : " << Plaque;
	imshow("Plates Detection", image);

	/*
	  __   __   __     _____     ______     ______
	 /\ \ / /  /\ \   /\  __-.  /\  ___\   /\  __ \
	 \ \ \'/   \ \ \  \ \ \/\ \ \ \  __\   \ \ \/\ \
	  \ \__|    \ \_\  \ \____-  \ \_____\  \ \_____\
	   \/_/      \/_/   \/____/   \/_____/   \/_____/
	*/

	/*
	cv::VideoCapture cap(0);
	cv::Mat image;

	if (!cap.isOpened())
		qDebug() << "Error opening video stream or file";

	 while (true) {
		 //// Image digester
		 cap.read(image);

		 //// Processing technique
		 std::vector<std::vector<cv::Point>> candidates = lp->locateCandidates(image);
		 lp->drawLicensePlate(image, candidates);

		 // Et ensuite la retouner afin de la renvoyer au superviseur/piloteur
		 // Créer un nouvel objet JSON avec la structure souhaitée
		 QJsonObject reponsePlaque;
		 QJsonArray tableauDonnees;
		 QString Plaque = lp->getLicensePlate();

		 if (!(Plaque == ""))
		 {
			 // Ajouter la plaque d'immatriculation au tableau
			 reponsePlaque["reponsePlaqueReco"] = Plaque;

			 // Conversion de l'objet JSON en chaîne JSON
			 QJsonDocument jsonDocument(reponsePlaque);
			 QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

			 if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
				 socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur
		 }

		 imshow("Plates Detection", image);
		 cv::waitKey(1);
	}

	cap.release();
	cv::destroyAllWindows();
	*/

	qDebug() << "Constructeur ClientTCPIP::ClientTCPIP(QObject* parent) : PASS";
	qDebug() << "/////////////////////////////////////\n";
}

// Destructeur
ClientTCPIP::~ClientTCPIP()
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Desctructeur ClientTCPIP::~ClientTCPIP() : CALL";

	delete lp;

	qDebug() << "Desctructeur ClientTCPIP::~ClientTCPIP() : PASS";
	qDebug() << "/////////////////////////////////////\n";
}