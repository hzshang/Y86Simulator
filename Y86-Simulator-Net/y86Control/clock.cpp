#include "clock.h"

Clock::Clock()
{
    server=new QTcpServer();
    for(int i=0;i<5;i++)
        socket[i]=NULL;
    stepIsOK=0;
}

void Clock::ready()
{
    server->listen(QHostAddress::Any,CLOCK_PORT);
    connect(server,SIGNAL(newConnection()),this,SLOT(on_newConnection()));
}

void Clock::nextStep()
{
    //向五个阶段发送数据，走一步
    QByteArray bytes=QString("nextStep").toUtf8();
    for(int i=4;i>=0;i--)
        socket[i]->write(bytes);
}
void Clock::restartPipeline()
{
    QByteArray bytes=QString("restart").toUtf8();
    for(int i=0;i<5;i++)
        socket[i]->write(bytes);
}

void Clock::on_newConnection()//每个阶段的clientToClock连接时调用
{
    int i;
    for(i=0;socket[i]&&i<5;)
        i++;
    if(i==5)
    {
        QMessageBox::warning(NULL,"Warning",QString("检测到重复阶段，请重新开始."),QMessageBox::Ok);
        server->pauseAccepting();
        return;
    }
    socket[i]=server->nextPendingConnection();
    connect(socket[i],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    if(i==4)
    {
        server->pauseAccepting();
        emit clockIsOK();
    }
}
void Clock::on_readyRead()
{
    stepIsOK++;
    if(stepIsOK==5)
    {
        emit stepIsDone();
        stepIsOK=0;
    }
}
