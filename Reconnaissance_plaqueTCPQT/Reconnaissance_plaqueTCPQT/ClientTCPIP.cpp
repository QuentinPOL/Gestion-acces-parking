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
	socketClient->connectToHost("192.168.65.210", 1234);
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
		/*
		  __   __   __     _____     ______     ______
		 /\ \ / /  /\ \   /\  __-.  /\  ___\   /\  __ \
		 \ \ \'/   \ \ \  \ \ \/\ \ \ \  __\   \ \ \/\ \
		  \ \__|    \ \_\  \ \____-  \ \_____\  \ \_____\
		   \/_/      \/_/   \/____/   \/_____/   \/_____/
		*/

		cv::VideoCapture cap("rtsp://admin:Admin@1234@192.168.64.178:554/cam/realmonitor?channel=1&subtype=0");
		cv::Mat image;

		if (!cap.isOpened())
			qDebug() << "Probleme lors de l'ouverture du flux video";
		else {
			qDebug() << "Flux video ouvert avec succes";

			int count = 0;
			QDateTime startTime = QDateTime::currentDateTime(); // Enregistrer le temps de démarrage

			while (startTime.secsTo(QDateTime::currentDateTime()) < 15) { // Boucle tant que le temps écoulé est inférieur à 15 secondes
				count++;
				cap.read(image);

				if (count == 40) {
					std::vector<std::vector<cv::Point>> candidates = lp->locateCandidates(image);
					lp->drawLicensePlate(image, candidates);

					QJsonObject reponsePlaque;
					QJsonArray tableauDonnees;
					QString Plaque = lp->getLicensePlate();
					reponsePlaque["reponsePlaqueReco"] = "AA-508-CP";

					QJsonDocument jsonDocument(reponsePlaque);
					QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

					if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
					{
						socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur
						break;
					}

					if (!(Plaque == "")) {
						qDebug() << "Une plaque a était trouvé";

						reponsePlaque["reponsePlaqueReco"] = lp->getLicensePlate();

						QJsonDocument jsonDocument(reponsePlaque);
						QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);

						if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connecté
						{
							socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur
							break;
						}
					}

					count = 0;
				}

				imshow("Plates Detection", image);
				cv::waitKey(1);
			}
		}

		cap.release();
		cv::destroyAllWindows();
	}
	else if (jsonMessage.contains("DemandeEtatCamera") && jsonMessage["DemandeEtatCamera"].toString() == "DemandeEtat") // si on recoit une demande d'état de la caméra
	{

	}
}