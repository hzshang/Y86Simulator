#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "y86Control/decode.h"
#include "y86Control/execute.h"
#include "y86Control/fetch.h"
#include "y86Control/memory.h"
#include "y86Control/writeback.h"
#include <QFile>
#include <QFileDialog>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    Fetch* f;
    Decode* d;
    Execute* e;
    Memory* m;
    Writeback* w;
    QObject::connect(this,SIGNAL(sendInstr(QString)),f,SLOT(receiveInstr(QString)));
    
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

void MainWindow::readFile()
{
    QFile* loadFile = new QFile(fileName);
    if(!loadFile->open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning()<<"open file error!";
        return;
    }
    QTextStream in(loadFile);
    instrCode =  in.readAll();
    ui->instrustion->setText(instrCode);
    emit sendInstr(instrCode);
}

void MainWindow::on_openFile_clicked()
{
    openFile();
    readFile();
}
