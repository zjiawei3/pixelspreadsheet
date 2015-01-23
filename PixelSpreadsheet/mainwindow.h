#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QDebug>
#include <QRgb>
using namespace std;
namespace Ui {
    class MainWindow;
}
struct char5{
    int x,y,r,g,b;
};
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    QString imageFilename, spreadsheetFilename;
    vector <char5> v;
    int maxX, maxY;
    QImage global_img;
    int rIndex, gIndex, bIndex;

private:
    Ui::MainWindow *ui;
    void displayImage(QImage a);
    void convertSSToImage();
    void convertImageToSS(bool shrink);
    void activateButtons(QString type);
    void updateStatus(QString type);
    void deleteStatus(QString type);
    void deactivateButtons(QString type);
private slots:
    void on_spreadsheetToImage_clicked();
    void on_loadSpreadsheet_clicked();
    void on_imageToSpreadsheet_clicked();
    void on_loadImage_clicked();
    void displayImage(QString filename);
    void displaySS();
    void on_saveImage_clicked();
    void on_saveSheet_clicked();

};

#endif // MAINWINDOW_H
