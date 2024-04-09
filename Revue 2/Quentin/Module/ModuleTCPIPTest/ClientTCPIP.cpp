#include "ClientTCPIP.h"

// Constructeur
ClientTCPIP::ClientTCPIP(QObject* parent) : QObject(parent)
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Constructeur ClientTCPIP::ClientTCPIP(QObject* parent) : CALL";

	if (socketClient = new QTcpSocket(this))
		qDebug() << "Constructeur QTcpSocket : PASS";

	QObject::connect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On connecte nos signaux et slots avec le signal connected
	QObject::connect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On connecte nos signaux et slots avec le signal disconnected
	QObject::connect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On connecte nos signaux et slots avec le signal readyRead

	// On va essayer de se connecter au serveur
	socketClient->connectToHost("192.168.64.91 ", 1234);
	qDebug() << "Constructeur ClientTCPIP::ClientTCPIP(QObject* parent) : PASS";
	qDebug() << "/////////////////////////////////////\n";
}

// Destructeur
ClientTCPIP::~ClientTCPIP()
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Desctructeur ClientTCPIP::~ClientTCPIP() : CALL";

	delete socketClient; // On Supprime pour la m�moire

	qDebug() << "Desctructeur ClientTCPIP::~ClientTCPIP() : PASS";
	qDebug() << "/////////////////////////////////////\n";
}

// M�thode du slot lorsque le signal connected est �mis par socketClient
void ClientTCPIP::onSocketConnected()
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Slot ClientTCPIP::onSocketConnected() : CALL";

	if (socketClient->state() == QTcpSocket::ConnectedState) // Si on est connecter
	{
		qDebug() << "Methode socketClient->state() : Connexion au serveur reussi !";
		qDebug() << "Status connexion : Connecter";
	}
	else
	{
		qDebug() << "Methode socketClient->state() : Probleme lors de la connexion au serveur !";
		qDebug() << "Status connexion : Non Connecter";
	}

	qDebug() << "Slot ClientTCPIP::onSocketConnected() : PASS";
	qDebug() << "/////////////////////////////////////\n";
}

// M�thode du slot lorsque le signal disconnected est �mis par socketClient
void ClientTCPIP::onSocketDisconnected()
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Slot ClientTCPIP::onSocketDisconnected() : CALL";

	QObject::disconnect(socketClient, SIGNAL(connected()), this, SLOT(onSocketConnected())); // On deconnecte nos signaux et slots avec le signal connected
	QObject::disconnect(socketClient, SIGNAL(disconnected()), this, SLOT(onSocketDisconnected())); // On deconnecte nos signaux et slots avec le signal disconnected
	QObject::disconnect(socketClient, SIGNAL(readyRead()), this, SLOT(onSocketReadyRead())); // On deconnecte nos signaux et slots avec le signal readyRead

	socketClient->close(); // On ferme la connexion

	qDebug() << "Status connexion : Deconnecter";
	qDebug() << "Slot ClientTCPIP::onSocketDisconnected() : PASS";
	qDebug() << "/////////////////////////////////////\n";
}

// M�thode du slot lorsque le signal readyRead est �mis par socketClient
void ClientTCPIP::onSocketReadyRead()
{
	qDebug() << "/////////////////////////////////////";
	qDebug() << "Slot ClientTCPIP::onSocketReadyRead() : CALL";

	QByteArray data = socketClient->read(socketClient->bytesAvailable()); // On va lire les donn�es re�u
	QString str(data); // On converti nos donn�es en une chaine de caract�re

	// Ensuite les affiches dans la console
	qDebug() << "---------------- MESSAGE SERVEUR -----------------------------";
	qDebug() << str;
	qDebug() << "---------------- MESSAGE SERVEUR -----------------------------";

	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // On d�code en objet JSON    
	if (jsonMessage.contains("DemandeRecoPlaque") && jsonMessage["DemandeRecoPlaque"].toString() == "DemandeOUI") // si on recoit une demande de reconnaissance de plaque
	{
		// Et ensuite la retouner afin de la renvoyer au superviseur/piloteur
		// Cr�er un nouvel objet JSON avec la structure souhait�e
		QJsonObject reponsePlaque;
		QJsonArray tableauDonnees;
		QString Plaque = "WW-245-WW";

		// Ajouter la plaque d'immatriculation au tableau
		reponsePlaque["reponsePlaqueReco"] = Plaque;

		// Conversion de l'objet JSON en cha�ne JSON
		QJsonDocument jsonDocument(reponsePlaque);
		QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une cha�ne JSON minimis�e

		if (socketClient->state() == QTcpSocket::ConnectedState) // Si le socket est bien connect�
		{
			socketClient->write(jsonString.toUtf8()); // On envoie le message au serveur
			qDebug() << "Envoie du message au serveur :" << jsonString;
		}
		else
			qDebug() << "Probleme de l'envoie du message au serveur :" << jsonString;
	}

	qDebug() << "Slot ClientTCPIP::onSocketReadyRead() : PASS";
	qDebug() << "/////////////////////////////////////\n";
}