#include <QtGui/QApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"
//#include "cv.h"
//#include "highgui.h"
#include <QFile>
#include <QDebug>
#include <vector>
using namespace std;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}
