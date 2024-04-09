#include <QtCore/QCoreApplication>
#include "ServeurTCPIP.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

	ServeurTCPIP serveur;

    return a.exec();
}
