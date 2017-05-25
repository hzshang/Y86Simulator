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
    QObject::connect(f,SIGNAL(sendFromFetch(QMap<QString,int>)),this,SLOT(receiveFromFetch(QMap<QString,int>)));
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
    emit sendInstr(instrCode);//给f阶段发送指令信号
}

void MainWindow::on_openFile_clicked()
{
    openFile();
    readFile();
}

void MainWindow::receiveFromFetch(QMap<QString,int> rev)
{ 
    ui->F_stat->clear();
    if(rev["stat"] == 0)
        ui->F_stat->setText("AOK");
    else if(stat == 1)
        ui->F_stat->setText("HTL");
    else if(stat == 3)
        ui->F_stat->setText("INS");
    ui->F_stat->setStyleSheet("background-color:rgba(0,255,255,255)");

    ui->PC->clear();
    if(rev.contains("PC"))
    {
        QString str = QString::number(rev["PC"],10);
        ui->PC->setText(str);
        ui->PC->setStyleSheet("background-color:rgba(0,255,255,255)");
    }

    ui->predPC->clear();
    if(rev.contains("predPC"))
    {
        QString str = QString::number(rev["predPC"],10);
        ui->predPC->setText(str);
        ui->predPC->setStyleSheet("background-color:rgba(0,255,255,255)");
    }

}
