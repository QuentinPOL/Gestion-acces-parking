#include "ClientTCPIP.h"

// Constructeur
ClientTCPIP::ClientTCPIP(QObject* parent) : QObject(parent)
{
	lp = new LicensePlate(); // Instantiate LicensePlate object
	socketClient = new QTcpSocket(this); // Inialisation du socket client pour notre interface (IHM)

	QObject::connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On connecte nos signaux et slots avec le signal connected
	QObject::connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On connecte nos signaux et slots avec le signal disconnected
	QObject::connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On connecte nos signaux et slots avec le signal readyRead

	// On va essayer de se connecter au serveur
	socketClient->connectToHost("192.168.64.91", 1234);
}

// Destructeur
ClientTCPIP::~ClientTCPIP()
{
	QObject::disconnect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On deconnecte nos signaux et slots avec le signal connected
	QObject::disconnect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On deconnecte nos signaux et slots avec le signal disconnected
	QObject::disconnect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On deconnecte nos signaux et slots avec le signal readyRead

	socketClient->close(); // On ferme la connexion
	delete socketClient; // On Supprime pour la mémoire
}

// Méthode du slot lorsque le signal connected est émis par socketClient
void ClientTCPIP::onSocketConnected()
{
	if (socketClient->state() == QTcpSocket::ConnectedState) // Si on est connecter
		qDebug() << "Connexion au serveur reussi !";
	else
		qDebug() << "Probleme lors de la connexion au serveur !";

	qDebug() << "Status connexion : Connecter";
}

// Méthode du slot lorsque le signal disconnected est émis par socketClient
void ClientTCPIP::onSocketDisconnected()
{
	qDebug() << "Status connexion : Deconnecter";
}

// Méthode du slot lorsque le signal readyRead est émis par socketClient
void ClientTCPIP::onSocketReadyRead()
{
	QByteArray data = socketClient->read(socketClient->bytesAvailable()); // On va lire les données reçu
	QString str(data); // On converti nos données en une chaine de caractère

	// Ensuite les affiches dans la console
	qDebug() << "---------------- MESSAGE SERVEUR -----------------------------";
	qDebug() << str;
	qDebug() << "---------------- MESSAGE SERVEUR -----------------------------";

	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // On décode en objet JSON    
	if (jsonMessage.contains("DemandeRecoPlaque") && jsonMessage["DemandeRecoPlaque"].toString() == "DemandeOUI") // si on recoit une demande de reconnaissance de plaque
	{
		// On va lancer le processus d'analyse de plaque [NE PAS OUBLIER DE FAIRE UNE CLASSE CAMERA]
		/*
		 __     __    __     ______     ______     ______
		/\ \   /\ "-./  \   /\  __ \   /\  ___\   /\  ___\
		\ \ \  \ \ \-./\ \  \ \  __ \  \ \ \__ \  \ \  __\
		 \ \_\  \ \_\ \ \_\  \ \_\ \_\  \ \_____\  \ \_____\
		  \/_/   \/_/  \/_/   \/_/\/_/   \/_____/   \/_____/
		*/

		std::string filename = "Resources/plaques-WW.jpg"; // A remplacer les frames de la caméra
		cv::Mat image;
		image = cv::imread(filename, cv::IMREAD_COLOR);

		if (!image.data)
			qDebug() << "ERREUR: IMPOSSIBLE DE LIRE L'IMAGE DEPUIS LE FICHIER\n\n";
		else
			qDebug() << "IMAGE CHARGER AVEC SUCCES\n\n";

		std::vector<std::vector<cv::Point>> candidates = lp->locateCandidates(image);
		lp->drawLicensePlate(image, candidates);

		imshow("Plates Detection", image);

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
		 //std::cout << "Error opening video stream or file" << std::endl;
	    }

	     while (true) {
		 //// Image digester
		 //cv::Mat image;
		 //cap >> image;
		 //if (image.empty()) break;

		 //// Keyboard listener
		 //char c = (char) cv::waitKey(1);
		 //if (c == 200) break;

		 //// Processing technique
		 //std::vector<std::vector<cv::Point>> candidates = lp.locateCandidates(image);

		 //cv::Mat drawing = cv::Mat::zeros(image.size(), CV_8UC3);
		 //std::vector<cv::Vec4i> hierarchy;
		 //for (int i = 0; i < candidates.size(); i++) {
		 //  cv::Scalar color = cv::Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
		 //  cv::drawContours(drawing, candidates, i, color, 2, 8, hierarchy, 0, cv::Point() );
		 //}
		 //cv::imshow("Drawing", drawing);
		 //cv::waitKey(0);

		 //lp.drawLicensePlate(image, candidates);
		 //lp.viewer(image, "Frame");
	    }

	    cap.release();
	    cv::destroyAllWindows();
	    */

		// Et ensuite la retouner afin de la renvoyer au superviseur/piloteur
		// Créer un nouvel objet JSON avec la structure souhaitée
		QJsonObject reponsePlaque;
		QJsonArray tableauDonnees;

		// Ajouter la plaque d'immatriculation au tableau
		reponsePlaque["reponsePlaqueReco"] = lp->getLicensePlate();

		// Conversion de l'objet JSON en chaîne JSON
		QJsonDocument jsonDocument(reponsePlaque);
		QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

		if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
			socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur
	}
}