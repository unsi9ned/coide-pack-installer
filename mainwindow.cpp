#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QApplication>
#include "packdescription.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    PackDescription pack;
#if 1
    parser.parse(QApplication::applicationDirPath() + "/" + "NordicSemiconductor.nRF_DeviceFamilyPack.pdsc", pack);
#elif 0
    parser.parse(QApplication::applicationDirPath() + "/" + "Keil.STM32F4xx_DFP.pdsc", pack);
#elif 1
    parser.parse(QApplication::applicationDirPath() + "/" + "Keil.SAMD21_DFP.pdsc", pack);
#elif 1
    parser.parse(QApplication::applicationDirPath() + "/" + "Microchip.SAMD21_DFP.pdsc", pack);
#endif
    pack.printInfo();
}

MainWindow::~MainWindow()
{
    delete ui;
}
