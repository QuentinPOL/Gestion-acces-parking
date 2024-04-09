#pragma once

#include <QDebug>
#include <QTcpSocket>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

class ClientTCPIP : public QObject
{
	Q_OBJECT
public:
	ClientTCPIP(QObject* parent = 0);
	~ClientTCPIP();

	QTcpSocket* socketClient; // Cr�ation du socket Client

public slots:
	void onSocketConnected(); // socketClient signal connecter
	void onSocketDisconnected(); // socketClient signal d�connecter
	void onSocketReadyRead(); // socketClient signal readyRead
};