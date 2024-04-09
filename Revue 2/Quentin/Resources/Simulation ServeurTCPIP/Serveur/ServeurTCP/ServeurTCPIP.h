#pragma once

#include <QObject>
#include <QDebug>
#include <QTcpSocket>
#include <QTcpServer>
#include "time.h"

class ServeurTCPIP : public QObject
{
	Q_OBJECT
public:
	ServeurTCPIP(QObject *parent = 0);
	~ServeurTCPIP();

	QTcpServer* socketServer; // Création du socket Serveur

public slots:
	void onServerNewConnection(); // Nouvelle connexion au serveur
	void onClientDisconnected(); // client signal déconnecter
	void onClientReadyRead(); // client signal readyRead
};

