#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFile>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::beginpipelineSlot(Y86 *y86)
{
    this->show();

}

void MainWindow::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty()){
        ui->filePath->setText(fileName);
        ui->filePath->setEnabled(false);
    }
}

void MainWindow::readfile()
{
    QFile loadFile = new QFile(fileName);
    if(!loadFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning()<<"open file error!";
        return;
    }
    QTextStream in(&loadFile);
    instrCode =  in.readAll();
    ui->instrustion->setText(instrCode);
}

void MainWindow::on_openFile_clicked()
{
    openFile();
    readFile();
}
