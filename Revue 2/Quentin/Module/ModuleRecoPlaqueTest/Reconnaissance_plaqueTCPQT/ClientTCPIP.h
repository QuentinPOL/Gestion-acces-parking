#pragma once

#include <QDebug>

#include "LicensePlate.h"

class ClientTCPIP : public QObject
{
	Q_OBJECT
public:
	ClientTCPIP(QObject* parent = 0);
	~ClientTCPIP();

	LicensePlate* lp; // Reconnaissance de plaque
};