#include "main.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <math.h>
#include <QFileDialog>
#include <QImageReader>
#include <QMessageBox>
#include <QImageIOPlugin>
//DEV: The following is added to support temp file
/*#include <stdio.h>
#include <istream>
#include <iostream>
#include <fstream>
#include <iterator>*/

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->progressBar->setVisible(false);
    ui->saveImage->setEnabled(false);
    ui->saveSheet->setEnabled(false);
    ui->imageToSpreadsheet->setEnabled(false);
    ui->spreadsheetToImage->setEnabled(false);
    ui->shrink->setEnabled(false);

    ui->image_memory_neg->setVisible(true);
    ui->image_save_neg->setVisible(true);
    ui->ss_memory_neg->setVisible(true);
    ui->ss_save_neg->setVisible(true);

    ui->image_memory_pos->setVisible(false);
    ui->image_save_pos->setVisible(false);
    ui->ss_memory_pos->setVisible(false);
    ui->ss_save_pos->setVisible(false);

    ui->image_to_ss_image->setEnabled(false);
    ui->ss_to_image_image->setEnabled(false);

    ui->loading_status_image->setVisible(false);
    ui->tableWidget->setVisible(false);
    global_img = QImage();
    qDebug() << QImageReader::supportedImageFormats ();
}

MainWindow::~MainWindow()
{
    delete ui;
}


//Opens a load file dialog. Then loads the image into the global_img image object
//Also updates the status to reflect the changes
void MainWindow::on_loadImage_clicked()
{

    imageFilename = QFileDialog::getOpenFileName(this, "Open Image",
                                                    "",
                                                    "Images (*.bmp *.gif *.ico *.jpeg *.jpg *.mng *.pbm *.pgm *.png *.ppm *.svg *.svgz *.tif *.tiff *.xbm *.xpm");
    if(imageFilename != "") //If the user entered a filename/path
    {
        global_img.load(imageFilename,0);
        displayImage(global_img);
        ui->imageBox->setTitle(QString("Image: " + imageFilename));
        activateButtons("image"); //Shows that an image was loaded into memory
        updateStatus("image");
        deactivateButtons("spreadsheet"); //Clears the previous spreadsheet data (if any) from memory
        deleteStatus("spreadsheet");
    }
}

void MainWindow::on_imageToSpreadsheet_clicked()
{
    convertImageToSS(ui->shrink->checkState()); //Converts Image to Spreadsheet, using shrink for Excel2002 compatibility if selected
}

//Opens a load file dialog. Then loads the spreadsheet into the v vector object
//Also updates the status to reflect the changes

void MainWindow::on_loadSpreadsheet_clicked()
{
    spreadsheetFilename = QFileDialog::getOpenFileName(this, "Open Image",
                                                    "" ,
                                                    "Spreadsheets (*.csv)");
    if(spreadsheetFilename != "") //If a valid filename/path
    {
        ui->spreadsheetBox->setTitle(QString("Spreadsheet: " + spreadsheetFilename));
        ui->loading_status_image->setVisible(true);

//DEV: Using temp file to support parallel w/r
/*  fstream f(spreadsheetFilename, fstream::in|fstream::binary);
  f << noskipws;
  istream_iterator<unsigned char> begin(f);
  istream_iterator<unsigned char> end;
  string tempfile ("output.temp");
  fstream f2(tempfile,fstream::out|fstream::trunc|fstream::binary);
  ostream_iterator<char> begin2(f2);
  copy(begin, end, begin2);
  spreadsheetFilename=tempfile;*/
//DEV: Temp End

        QApplication::processEvents(); //This processes the call to show the loading image before the loading begins
        v.clear(); //clears previous spreadsheet data (if any)
        maxX = 0;
        maxY = 0;
        int x,y;
        QFile csv_file(spreadsheetFilename); //opens the file for reading

        csv_file.open(QIODevice::ReadOnly);//DEV: Optional solution to open a file in use, Changed from ReadWrite

        if(!csv_file.isOpen()) //Catch statement if the file cannot be opened
        {
            qDebug() << "- Error, unable to open" << spreadsheetFilename << "for output";
        }
        QTextStream inStream(&csv_file);
        QString grabLine;
        QStringList list;
        char5 temp;
        rIndex = -1, gIndex = -1, bIndex = -1;
        int xIndex, yIndex;
        grabLine = inStream.readLine();
        list << grabLine.split(",");
        for(int i = 0; i < list.size(); i++ ) // The first line is read in. The program looks for x,y,r,g,b and sets the columns accordingly
        {
            if(list.at(i) == "x")
                xIndex = i;
            if(list.at(i) == "y")
                yIndex = i;
            if(list.at(i) == "r")
                rIndex = i;
            if(list.at(i) == "g")
                gIndex = i;
            if(list.at(i) == "b")
                bIndex = i;
        }
        while(!inStream.atEnd()){
            grabLine = inStream.readLine();
            list = grabLine.split(",");
            if(list.size() >= 0) {
                temp.x = list.at(xIndex).toInt();
                if(temp.x > maxX) maxX = temp.x; //The image's width and height are determined based on the maximum from the
                temp.y = list.at(yIndex).toInt();
                if(temp.y > maxY) maxY = temp.y;
                if(rIndex != -1 && rIndex < list.size())temp.r = list.at(rIndex).toInt();
                if(bIndex != -1 && bIndex < list.size())temp.b = list.at(bIndex).toInt();
                if(gIndex != -1 && gIndex < list.size())temp.g = list.at(gIndex).toInt();
                v.push_back(temp);
            }
        }
        for(int i =0; i < v.size(); i++)
        {
            x = v.at(i).x;
            y = v.at(i).y;
            if(x > maxX) x = maxX;
            if(y > maxY) y = maxY;
        }
        maxX += 1;
        maxY += 1; //to account for the 0 position offset
        ui->loading_status_image->setVisible(false);
        updateStatus("spreadsheet");
        activateButtons("spreadsheet");
        deactivateButtons("image");
        deleteStatus("image");
//DEV: Delete temp file
//remove(tempfile);
//DEV: Delete end
    }
}

//Updates the information displays with new information as files are loaded/converted
void MainWindow::updateStatus(QString type)
{
    if(type == "spreadsheet")
    {
        ui->resolution->setText(QString("Output Image: " + QString::number(maxX) + ", " + QString::number(maxY)));
        if(rIndex != -1) ui->red->setText("Red: Yes"); else ui->red->setText("Red: No");
        if(gIndex != -1) ui->green->setText("Green: Yes"); else ui->green->setText("Green: No");
        if(bIndex != -1) ui->blue->setText("Blue: Yes"); else ui->blue->setText("Blue: No");
    }
    if(type == "image")
    {
       int rows = global_img.height() * global_img.width();
       ui->outputRows->setText("Output Rows: " + QString::number(rows));
       ui->originalSize->setText("Original Size: " + QString::number(global_img.width()) + ", " + QString::number(global_img.height()));
       if(rows < 60000)ui->excelCompat->setText("Excel 2002: Ok"); else ui->excelCompat->setText("Excel 2002: Too Many Rows");
   }
}

//Sets information displays to default values
void MainWindow::deleteStatus(QString type)
{
    if(type == "spreadsheet")
    {
        ui->resolution->setText(QString("Spreadsheet not loaded"));
        ui->red->setText("Red: --");
        ui->green->setText("Green: --");
        ui->blue->setText("Blue: --");
    }
    if(type == "image")
    {
       ui->imageBox->setTitle(QString("No Image Loaded"));
       ui->outputRows->setText("Output Rows: -- ");
       ui->originalSize->setText("Original Size: -- ");
       ui->excelCompat->setText("Excel 2002: --");
   }
}


//Enabes the appropriate buttons depending on the state of the program
void MainWindow::activateButtons(QString type)
{
    if(type == "spreadsheet")
    {
        ui->saveSheet->setEnabled(true);
        ui->spreadsheetToImage->setEnabled(true);
        ui->ss_memory_pos->setVisible(true);
        ui->ss_memory_neg->setVisible(false);
        ui->ss_save_neg->setVisible(true);
        ui->ss_save_pos->setVisible(false);
        ui->ss_to_image_image->setEnabled(true);
        ui->tableWidget->setVisible(true);
    }
    if(type == "image")
    {
        ui->image_to_ss_image->setEnabled(true);
        ui->image_memory_pos->setVisible(true);
        ui->image_memory_neg->setVisible(false);
        ui->image_save_neg->setVisible(true);
        ui->image_save_pos->setVisible(false);
        ui->saveImage->setEnabled(true);
        ui->imageToSpreadsheet->setEnabled(true);
        ui->shrink->setEnabled(true);
        ui->imageView->setEnabled(true);
    }
}

//Disables buttons depending on the state of the program
void MainWindow::deactivateButtons(QString type)
{
    if(type == "spreadsheet")
    {
        ui->ss_to_image_image->setEnabled(false);
        ui->ss_memory_neg->setVisible(true);
        ui->ss_memory_pos->setVisible(false);
        ui->ss_save_neg->setVisible(true);
        ui->ss_save_pos->setVisible(false);
        ui->saveSheet->setEnabled(false);
        ui->spreadsheetToImage->setEnabled(false);
        ui->tableWidget->setVisible(false);
    }
    if(type == "image")
    {
        ui->image_to_ss_image->setEnabled(false);
        ui->image_memory_neg->setVisible(true);
        ui->image_memory_pos->setVisible(false);
        ui->image_save_neg->setVisible(true);
        ui->image_save_pos->setVisible(false);
        ui->saveImage->setEnabled(false);
        ui->imageToSpreadsheet->setEnabled(false);
        ui->shrink->setEnabled(false);
        ui->imageView->clear();
        ui->imageView->setEnabled(false);
    }
}

void MainWindow::on_spreadsheetToImage_clicked()
{
    convertSSToImage(); //Converts Spreadsheet to Image
}

//Opens save dialog and allows user to save an image
void MainWindow::on_saveImage_clicked(){

    QString filename = QFileDialog::getSaveFileName(this, "Save Image",
                                                        "" ,
                                                        "Jpeg (*.jpg);; Bmp (*.bmp);; Gif (*.gif);; Pgm (*.pgm);; Png (*.png);; Ppm (*.ppm);; Tif (*.tif)");
//Error handling
    if(filename!= "")
    {
        if(!global_img.save(QString(filename ),0,-1))
            QMessageBox::warning(this,"Warning","Warning: Image not Saved");
        ui->image_save_pos->setVisible(true);
        ui->image_save_neg->setVisible(false);
    }
}

//Opens save dialog and allows user to save a spreadsheet
void MainWindow::on_saveSheet_clicked(){

    QString filename = QFileDialog::getSaveFileName(this, "Save Image",
                                                        "" ,
                                                        "Image (*.csv)");
    if(filename != "")
    {
        QFile csv_file(filename);
        csv_file.open(QIODevice::ReadWrite);
        if(!csv_file.isOpen())
        {
        qDebug() << "- Error, unable to open" << filename << "for output";
        }
        QTextStream outStream(&csv_file);
//DEV: Change orders of RGB by Modification here

        outStream << "x,y,r,g,b\n";
        int r, g, b,x,y;
        for(int i = 0; i < v.size(); i++){
                r = v.at(i).r;
                b = v.at(i).b;
                g = v.at(i).g;
                x = v.at(i).x;
                y = v.at(i).y;

//DEV: and here.

                outStream<<x<<","<<y<<","<< r <<
                        ","<< g << ","<< b <<"\n";
        }
        activateButtons("spreadsheet");
        csv_file.close();
        ui->ss_save_pos->setVisible(true);
        ui->ss_save_neg->setVisible(false);
    }
}

//Takes the loaded image and passes it to displayImage(QImage a)
void MainWindow::displayImage(QString filename)
{
    QImage a = QImage(filename);
    displayImage(a);
}

//Displays Image a in the image window
void MainWindow::displayImage(QImage a)
{
    ui->imageView->setPixmap(QPixmap::fromImage(a));
    ui->imageView->setScaledContents(true);
}

//Displays spreadsheet information in window
void MainWindow::displaySS(){
    ui->tableWidget->clear();
    ui->tableWidget->insertRow(0);
    ui->tableWidget->setItem(0,0,0);
}


void MainWindow::convertSSToImage()
{
    //converts target spreadsheet to image
    char5 temp;

    //IplImage * img = cvCreateImage(cvSize(maxX,maxY),8,3);
    global_img = QImage(QSize(maxX,maxY),QImage::Format_RGB888);
    int x,y;
    char r,g,b;
    for(int i =0; i < v.size(); i++)
    {
        x = v.at(i).x;
        y = v.at(i).y;
        r = v.at(i).r;
        g = v.at(i).g;
        b = v.at(i).b;
        global_img.setPixel(x,y,qRgb(r,g,b));
    }
    displayImage(global_img);
    activateButtons("image");
    updateStatus("image");
}

void MainWindow::convertImageToSS(bool shrink)
{
    //converts image to spreadsheet
    float abs_max = 1000000; // All newer excel versions cap at around 1,000,000 lines. This code will auto-scale down
    float MAX_ROWS = 60000; //Scaled size to fit in older excel versions (and OpenOffice)
    v.clear();
    global_img;

    //Checks if and image is too large and shrinks it if this is the case
    if(global_img.height() * global_img.width() > abs_max)
    {
        float scale_factor = sqrt(abs_max/(global_img.height()*global_img.width()));
        global_img = global_img.scaledToWidth(global_img.width()*scale_factor,Qt::FastTransformation);
        QMessageBox::warning(this, "Max Size Reached", "This image will not fit into excel. It has been reduced to 1 million pixels so that data loss will not occur");
    }

    //Shrink funstion
    if(shrink){
        if(global_img.height() * global_img.width() > MAX_ROWS)
        {
            float scale_factor = sqrt(MAX_ROWS/(global_img.height()*global_img.width()));
            global_img = global_img.scaledToWidth(global_img.width()*scale_factor,Qt::FastTransformation);
        }
    }
    //char5 temp;

    QRgb temp;
    QColor color_temp;
    char5 temp_vec;
    int r; int g; int b; int a;
    for(int x = 0; x < global_img.width(); x++){
        for(int y = 0; y < global_img.height(); y++){
            temp = global_img.pixel(x,y);
            color_temp = QColor(temp);
            color_temp.getRgb(&r,&g,&b,&a);
            temp_vec.x = x;
            temp_vec.y = y;
            temp_vec.b = b;
            temp_vec.g = g;
            temp_vec.r = r;
            v.push_back(temp_vec);
            }
    }
    maxX = global_img.width();
    maxY = global_img.height();
    activateButtons("spreadsheet");
    updateStatus("spreadsheet");
}
