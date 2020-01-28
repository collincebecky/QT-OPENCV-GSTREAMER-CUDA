
#include <QApplication>
#include "mainwindow.h"
bool MainWindow::signal_recieved = false;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
