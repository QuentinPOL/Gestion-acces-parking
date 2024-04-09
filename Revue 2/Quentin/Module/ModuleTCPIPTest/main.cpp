#include <QtCore/QCoreApplication>
#include "ClientTCPIP.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    ClientTCPIP communication; // Classe principale

    return a.exec();
}
