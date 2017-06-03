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
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::beginpipelineSlot(Y86 *y86)
{
    qWarning()<<"MainWindow"<<QThread::currentThreadId();
    this->show();
    wy86=y86;
    connect(this,SIGNAL(PipelineRestart()),wy86,SLOT(on_PipelineRestart()));
    connect(this,SIGNAL(PipelineRun()),wy86,SLOT(on_PipelineRun()));
    connect(this,SIGNAL(PipelineStep()),wy86,SLOT(on_PipelineStep()));
    connect(this,SIGNAL(PipelineStop()),wy86,SLOT(on_PipelineStop()));
    connect(ui->horizontalSlider,SIGNAL(sliderMoved(int)),wy86,SLOT(changeCircleTime(int)));

    if(wy86->fetch)
    {
        QObject::connect(this,SIGNAL(sendInstr(QString)),wy86->fetch,SLOT(receiveInstr(QString)));
        QObject::connect(wy86->fetch,SIGNAL(sendFromFetch(QJsonObject)),this,SLOT(receiveFromFetch(QJsonObject)));
    }
    if(wy86->decode)
        QObject::connect(wy86->decode,SIGNAL(sendFromDecode(QJsonObject)),this,SLOT(receiveFromDecode(QJsonObject)));
    if(wy86->execute)
    {
        QObject::connect(wy86->execute,SIGNAL(sendCC(QJsonObject)),this,SLOT(receiveCC(QJsonObject)));
        QObject::connect(wy86->execute,SIGNAL(sendFromExecute(QJsonObject)),this,SLOT(receiveFromExecute(QJsonObject)));
    }
    if(wy86->memory)
    {
        QObject::connect(wy86->memory,SIGNAL(sendFromMemory(QJsonObject)),this,SLOT(receiveFromMemory(QJsonObject)));
    }
    if(wy86->writeback)
    {
        QObject::connect(wy86->writeback,SIGNAL(sendFromWriteback(QJsonObject)),this,SLOT(receiveFromWriteback(QJsonObject)));
        QObject::connect(wy86->writeback,SIGNAL(clearReg(bool)),this,SLOT(receiveClear(bool)));
    }
    if(!y86->master)
    {
        ui->horizontalSlider->setEnabled(false);
        ui->openFile->setEnabled(false);
        ui->restart->setEnabled(false);
        ui->next->setEnabled(false);
        ui->run->setEnabled(false);
    }
}

void MainWindow::openFile()
{
    fileName = QFileDialog::getOpenFileName(this);
    if(!fileName.isEmpty()){
        ui->filePath->setText(fileName);
        ui->filePath->setEnabled(false);
    }
}
void MainWindow::setLine(QLineEdit *l, QString str)
{
    l->setText(str);
    //l->setStyleSheet("background-color:rgba(0,255,255,255)");
}
void MainWindow::writeReg(QString dst, QString val)
{
    int d = dst.toInt();
    //int v = val.toInt();
    switch (d) {
    case 0:
        ui->eax->clear();
        ui->eax->setText(val);
        break;
    case 1:
        ui->ecx->clear();
        ui->ecx->setText(val);
        break;
    case 2:
        ui->edx->clear();
        ui->edx->setText(val);
        break;
    case 3:
        ui->ebx->clear();
        ui->ebx->setText(val);
        break;
    case 4:
        ui->esp->clear();
        ui->esp->setText(val);
        break;
    case 5:
        ui->ebp->clear();
        ui->ebp->setText(val);
        break;
    case 6:
        ui->esi->clear();
        ui->esi->setText(val);
        break;
    case 7:
        ui->edi->clear();
        ui->edi->setText(val);
        break;
    default:
        break;
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

void MainWindow::receiveFromFetch(QJsonObject rev)
{ 
    ui->F_stat->clear();
    if(rev.contains("stat"))
    {
        int stat = rev.value("stat").toInt();
        if(stat == 0)
        {
            setLine(ui->F_stat,"AOK");

            ui->F_instruction->clear();
            ui->F_instruction->setText(rev.value("instruction").toString());

            ui->PC->clear();
            if(rev.contains("PC"))
            {
                QString str = QString::number(rev.value("PC").toInt(),10);
                setLine(ui->PC,str);
            }

            ui->predPC->clear();
            if(rev.contains("predPC"))
            {
                QString str = QString::number(rev.value("predPC").toInt(),10);
                setLine(ui->predPC,str);
            }
        }
        else if(stat == 1)
            setLine(ui->F_stat,"HTL");
        else
            setLine(ui->F_stat,"INS");
    }
    else
    {
        ui->F_instruction->clear();
        ui->PC->clear();
        ui->predPC->clear();
    }
}

void MainWindow::receiveFromDecode(QJsonObject rev)
{
    ui->D_stat->clear();
    if(rev.contains("D_stat"))
    {
        int stat = rev.value("D_stat").toInt();
        if(stat == 0)
            setLine(ui->D_stat,"AOK");
        else if(stat == 1)
            setLine(ui->D_stat,"HTL");
        else
            setLine(ui->D_stat,"INS");
    }

    ui->D_icode->clear();
    ui->D_ifun->clear();
    ui->D_valP->clear();
    ui->D_instruction->clear();
    if(rev.contains("D_icode"))
    {
        QString str = QString::number(rev.value("D_icode").toInt(),10);
        setLine(ui->D_icode,str);

        str = QString::number(rev.value("D_ifun").toInt(),10);
        setLine(ui->D_ifun,str);

        str = QString::number(rev.value("D_valP").toInt(),10);
        setLine(ui->D_valP,str);

        ui->D_instruction->setText(rev.value("instruction").toString());

    }

    ui->D_rA->clear();
    if(rev.contains("D_rA"))
    {
        QString str = QString::number(rev.value("D_rA").toInt(),10);
        setLine(ui->D_rA,str);
    }
    ui->D_rB->clear();
    if(rev.contains("D_rB"))
    {
        QString str = QString::number(rev.value("D_rB").toInt(),10);
        setLine(ui->D_rB,str);
    }
    ui->D_valC->clear();
    if(rev.contains("D_valC"))
    {
        QString str = QString::number(rev.value("D_valC").toInt(),10);
        setLine(ui->D_valC,str);
    }
}

void MainWindow::receiveFromExecute(QJsonObject rev)
{
    ui->E_stat->clear();
    if(rev.contains("E_stat"))
    {
        int stat = rev.value("E_stat").toInt();
        if(stat == 0)
            setLine(ui->E_stat,"AOK");
        else if(stat == 1)
            setLine(ui->E_stat,"HTL");
        else
            setLine(ui->E_stat,"INS");
    }
    else
    {
        ui->ZF->clear();
        ui->SF->clear();
        ui->OF->clear();
    }

    ui->E_icode->clear();
    ui->E_ifun->clear();
    ui->E_instruction->clear();
    if(rev.contains("E_icode"))
    {
        QString str = QString::number(rev.value("E_icode").toInt(),10);
        setLine(ui->E_icode,str);
        str = QString::number(rev.value("E_ifun").toInt(),10);
        setLine(ui->E_ifun,str);

        ui->E_instruction->setText(rev.value("instruction").toString());
    }
    ui->E_valC->clear();
    if(rev.contains("E_valC"))
    {
        QString str = QString::number(rev.value("E_valC").toInt(),10);
        setLine(ui->E_valC,str);
    }
    ui->E_valA->clear();
    if(rev.contains("E_valA"))
    {
        QString str = QString::number(rev.value("E_valA").toInt(),10);
        setLine(ui->E_valA,str);
    }
    ui->E_valB->clear();
    if(rev.contains("E_valB"))
    {
        QString str = QString::number(rev.value("E_valB").toInt(),10);
        setLine(ui->E_valB,str);
    }
    ui->E_dstE->clear();
    if(rev.contains("E_dstE"))
    {
        QString str = QString::number(rev.value("E_dstE").toInt(),10);
        setLine(ui->E_dstE,str);
    }
    ui->E_dstM->clear();
    if(rev.contains("E_dstM"))
    {
        QString str = QString::number(rev.value("E_dstM").toInt(),10);
        setLine(ui->E_dstM,str);
    }
    ui->E_srcA->clear();
    if(rev.contains("E_srcA"))
    {
        QString str = QString::number(rev.value("E_srcA").toInt(),10);
        setLine(ui->E_srcA,str);
    }
    ui->E_srcB->clear();
    if(rev.contains("E_srcB"))
    {
        QString str = QString::number(rev.value("E_srcB").toInt(),10);
        setLine(ui->E_srcB,str);
    }
}

void MainWindow::receiveFromMemory(QJsonObject rev)
{
    ui->M_stat->clear();
    if(rev.contains("M_stat"))
    {
        int stat = rev.value("M_stat").toInt();
        if(stat == 0)
            setLine(ui->M_stat,"AOK");
        else if(stat == 1)
            setLine(ui->M_stat,"HTL");
        else
            setLine(ui->M_stat,"INS");
    }

    ui->M_icode->clear();
    ui->M_instruction->clear();
    if(rev.contains("M_icode"))
    {
        QString str = QString::number(rev.value("M_icode").toInt(),10);
        setLine(ui->M_icode,str);

        ui->M_instruction->setText(rev.value("instruction").toString());
    }
    ui->M_Cnd->clear();
    if(rev.contains("M_Cnd"))
    {
        QString str = QString::number(rev.value("M_Cnd").toInt(),10);
        setLine(ui->M_Cnd,str);
    }

    ui->M_valE->clear();
    if(rev.contains("M_valE"))
    {
        QString str = QString::number(rev.value("M_valE").toInt(),10);
        setLine(ui->M_valE,str);
    }
    ui->M_valA->clear();
    if(rev.contains("M_valA"))
    {
        QString str = QString::number(rev.value("M_valA").toInt(),10);
        setLine(ui->M_valA,str);
    }
    ui->M_dstE->clear();
    if(rev.contains("M_dstE"))
    {
        QString str = QString::number(rev.value("M_dstE").toInt(),10);
        setLine(ui->M_dstE,str);
    }
    ui->M_dstM->clear();
    if(rev.contains("M_dstM"))
    {
        QString str = QString::number(rev.value("M_dstM").toInt(),10);
        setLine(ui->M_dstM,str);
    }
}

void MainWindow::receiveFromWriteback(QJsonObject rev)
{
    ui->W_stat->clear();
    if(rev.contains("W_stat"))
    {
        int stat = rev.value("W_stat").toInt();
        if(stat == 0)
            setLine(ui->W_stat,"AOK");
        else if(stat == 1)
            setLine(ui->W_stat,"HTL");
        else if(stat == 2)
            setLine(ui->W_stat,"ADS");
        else
            setLine(ui->W_stat,"INS");
    }

    ui->W_icode->clear();
    ui->W_instruction->clear();
    if(rev.contains("W_icode"))
    {
        QString str = QString::number(rev.value("W_icode").toInt(),10);
        setLine(ui->W_icode,str);

        ui->W_instruction->setText(rev.value("instruction").toString());
    }

    ui->W_dstE->clear();
    ui->W_valE->clear();
    if(rev.contains("W_dstE"))
    {
        QString dst = QString::number(rev.value("W_dstE").toInt(),10);
        setLine(ui->W_dstE,dst);
        QString val = QString::number(rev.value("W_valE").toInt(),10);
        setLine(ui->W_valE,val);
        writeReg(dst,val);
    }
    ui->W_dstM->clear();
    ui->W_valM->clear();
    if(rev.contains("W_dstM"))
    {
        QString dst = QString::number(rev.value("W_dstM").toInt(),10);
        setLine(ui->W_dstM,dst);
        QString val = QString::number(rev.value("W_valM").toInt(),10);
        setLine(ui->W_valM,val);
        writeReg(dst,val);
    }
}

void MainWindow::receiveCC(QJsonObject rev)
{
    ui->ZF->clear();
    ui->SF->clear();
    ui->OF->clear();
    if(rev.contains("SF"))
    {
        QString str = QString::number(rev.value("ZF").toInt(),10);
        ui->ZF->setText(str);
        str = QString::number(rev.value("SF").toInt(),10);
        ui->SF->setText(str);
        str = QString::number(rev.value("OF").toInt(),10);
        ui->OF->setText(str);
    }
}
void MainWindow::receiveClear(bool f)
{
    if(f)
    {
        ui->eax->clear();
        ui->ebx->clear();
        ui->ecx->clear();
        ui->edx->clear();
        ui->esp->clear();
        ui->ebp->clear();
        ui->esi->clear();
        ui->edi->clear();
    }
}

void MainWindow::on_next_clicked()
{
    emit PipelineStep();
    ui->run->setText("Run");
}

void MainWindow::on_run_clicked()
{
    if(ui->run->text()=="Run")
    {
        ui->run->setText("Stop");
        emit PipelineRun();
    }
    else
    {
        ui->run->setText("Run");
        emit PipelineStop();
    }
}

void MainWindow::on_restart_clicked()
{
    ui->run->setText("Run");
    emit PipelineRestart();
}
