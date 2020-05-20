#include <QApplication>
#include <QThread>
#include "istatenetworkwidget.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    IstateNetworkWidget networkWidget;
    networkWidget.show();

    return a.exec();
}
