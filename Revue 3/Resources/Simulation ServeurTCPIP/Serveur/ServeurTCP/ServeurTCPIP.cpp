#include "ServeurTCPIP.h"

#include <iostream>
#include <QJsonObject>
#include <QJsonDocument>

// Constructeur 
ServeurTCPIP::ServeurTCPIP(QObject* parent) : QObject(parent)
{
	socketServer = new QTcpServer(this); // Inialisation du socket server pour notre interface (IHM)
	connect(socketServer, SIGNAL(newConnection()), this, SLOT(onServerNewConnection())); // On connecte nos signaux et slots avec le signal newConnection

	// On ouvre le serveur sur n'importe quelle adresse ipv4 (127.0.0.1 ou ip sur le réseaux) et sur le port 1234s
	if (socketServer->listen(QHostAddress::AnyIPv4, 1234))
		qDebug() << "Ouverture du serveur sur le port 1234";
	else
		qDebug() << "ERREUR lors de l'ouverture du port";
}

// Destructeur 
ServeurTCPIP::~ServeurTCPIP()
{
	socketServer->close(); // On ferme la connexion
	delete socketServer; // On Supprime pour la mémoire
}

// Méthode du slot lorsque le signal newConnection est émis par socketServer
void ServeurTCPIP::onServerNewConnection()
{
	QTcpSocket* client = socketServer->nextPendingConnection(); // On récupère le client

	connect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On connecte les signaux et slots du client avec le signal readyRead
	connect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On connecte les signaux et slots du client avec le signal disconnected

	// Et ensuite envoyer une demande de reconnaissance de plaque
	// Créer un nouvel objet JSON avec la structure souhaitée
	QJsonObject demandePlaqueReco;
	QByteArray tableauDonnees;

	if (client->peerAddress() == QHostAddress("192.168.65.140"))
		qDebug() << "Arduino Capteur/FeuBicolore connecter (192.168.65.140)";
	else
		qDebug() << "Un client s'est connecter : " << client->peerAddress();

	// Ajouter le tableau au nouvel objet
	demandePlaqueReco["DemandeRecoPlaque"] = "DemandeOUI";

	// Conversion de l'objet JSON en chaîne de caracteres
	QJsonDocument jsonDocument(demandePlaqueReco);
	QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

	// Reconversion de la QString en QByteArray pour l'envoie du message
	tableauDonnees = jsonString.toUtf8();

	qDebug() << "Message envoyer au client : " << tableauDonnees;

	if (client->state() == QTcpSocket::ConnectedState) // Si le client est bien connecté
		client->write(tableauDonnees); // on lui renvoie un message en réponse
}

// Méthode du slot lorsque le signal disconnected est émis par client
void ServeurTCPIP::onClientDisconnected()
{
	QTcpSocket* client = qobject_cast<QTcpSocket*>(sender()); // On va récupèrer l'objet qui nous un envoyé le signal (conversion car on reçoie QObject* et il nous faut QTcpSocket*)

	if (client->peerAddress() == QHostAddress("192.168.65.140"))
		qDebug() << "Arduino Capteur/FeuBicolore deconnecter";
	else
		qDebug() << "Un client s'est deconnecter : " << client->peerAddress();

	disconnect(client, SIGNAL(readyRead()), this, SLOT(onClientReadyRead())); // On déconnecte les signaux et slots du client avec le signal readyRead
	disconnect(client, SIGNAL(disconnected()), this, SLOT(onClientDisconnected())); // On déconnecte les signaux et slots du client avec le signal disconnected

	client->deleteLater(); // Suppresion de l'object pour libérer la mémoire
}

// Méthode du slot lorsque le signal readyRead est émis par client
void ServeurTCPIP::onClientReadyRead()
{
	QTcpSocket* obj = qobject_cast<QTcpSocket*>(sender()); // On va récupèrer l'objet qui nous un envoyé le signal (conversion car on reçoie QObject* et il nous faut QTcpSocket*)

	QByteArray data = obj->read(obj->bytesAvailable()); // On va lire les données reçu
	QString str(data); // On converti nos données en une chaine de caractère

	QJsonObject jsonMessage = QJsonDocument::fromJson(str.toUtf8()).object(); // On décode en objet JSON      
	QJsonObject reponse; //  On créer la réponse en objet JSON

	qDebug() << "---------------- MESSAGE CLIENT -----------------------------";
	qDebug() << "Message recu d'un client : " << str << " [" << obj->peerAddress() << "]";
	qDebug() << "---------------- MESSAGE CLIENT -----------------------------";

	if (jsonMessage["InfoVeh"].toString() == "VehiculeDetecter") // Si y'a un véhicule de detecter
	{
		// Et ensuite envoyer une demande de reconnaissance de plaque
		// Créer un nouvel objet JSON avec la structure souhaitée
		QJsonObject demandePlaqueReco;
		QByteArray tableauDonnees;

		// Ajouter le tableau au nouvel objet
		demandePlaqueReco["Barriere"] = "Ouverture";

		// Conversion de l'objet JSON en chaîne de caracteres
		QJsonDocument jsonDocument(demandePlaqueReco);
		QString jsonString = jsonDocument.toJson(QJsonDocument::Compact);  // Compact pour une chaîne JSON minimisée

		// Reconversion de la QString en QByteArray pour l'envoie du message
		tableauDonnees = jsonString.toUtf8();

		qDebug() << "Message envoyer au client : " << tableauDonnees;

		if (obj->state() == QTcpSocket::ConnectedState) // Si le client est bien connecté
			obj->write(tableauDonnees); // on lui renvoie un message en réponse
	}
}
