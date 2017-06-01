#include "clock.h"

Clock::Clock(QWaitCondition *wait)
{
    for(int i=0;i<5;i++)
        server[i]=new QTcpServer();
    for(int i=0;i<5;i++)
        socket[i]=NULL;
    stepIsOK=0;
    y86Awake=wait;
}

void Clock::ready()
{
    server[0]->listen(QHostAddress::Any,CLOCK_FOR_FETCH);
    server[1]->listen(QHostAddress::Any,CLOCK_FOR_Decode);
    server[2]->listen(QHostAddress::Any,CLOCK_FOR_Execute);
    server[3]->listen(QHostAddress::Any,CLOCK_FOR_Memory);
    server[4]->listen(QHostAddress::Any,CLOCK_FOR_Writeabck);
    connect(server[0],SIGNAL(newConnection()),this,SLOT(on_FetchConnection()),Qt::DirectConnection);
    connect(server[1],SIGNAL(newConnection()),this,SLOT(on_DecodeConnection()),Qt::DirectConnection);
    connect(server[2],SIGNAL(newConnection()),this,SLOT(on_ExecuteConnection()),Qt::DirectConnection);
    connect(server[3],SIGNAL(newConnection()),this,SLOT(on_MemoryConnection()),Qt::DirectConnection);
    connect(server[4],SIGNAL(newConnection()),this,SLOT(on_WritebackConnection()),Qt::DirectConnection);
}

void Clock::nextStep()
{
    for(int i=4;i>=0;i--)
    {
        socket[i]->write("nextStep");
        socket[i]->waitForBytesWritten();
    }
}
void Clock::restartPipeline()
{
    for(int i=4;i>=0;i--)
    {
        socket[i]->write("restart");
        socket[i]->waitForBytesWritten();
    }
}
void Clock::run()
{
    qWarning()<<"CLOCK"<<QThread::currentThreadId();
    exec();
}

void Clock::move()
{
    moveToThread(this);
    for(int i=0;i<5;i++)
        server[i]->moveToThread(this);
}

void Clock::on_FetchConnection()
{
    socket[0]=server[0]->nextPendingConnection();
    connect(socket[0],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    server[0]->pauseAccepting();
    if(socket[0]&&socket[1]&&socket[2]&&socket[3]&&socket[4]){
        emit clockIsOK();
    }
}

void Clock::on_DecodeConnection()
{
    socket[1]=server[1]->nextPendingConnection();
    connect(socket[1],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    server[1]->pauseAccepting();
    if(socket[0]&&socket[1]&&socket[2]&&socket[3]&&socket[4]){
        emit clockIsOK();
    }
}

void Clock::on_ExecuteConnection()
{
    socket[2]=server[2]->nextPendingConnection();
    connect(socket[2],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    server[2]->pauseAccepting();
    if(socket[0]&&socket[1]&&socket[2]&&socket[3]&&socket[4]){
        emit clockIsOK();
    }
}

void Clock::on_MemoryConnection()
{
    socket[3]=server[3]->nextPendingConnection();
    connect(socket[3],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    server[3]->pauseAccepting();
    if(socket[0]&&socket[1]&&socket[2]&&socket[3]&&socket[4]){
        emit clockIsOK();
    }
}

void Clock::on_WritebackConnection()
{
    socket[4]=server[4]->nextPendingConnection();
    connect(socket[4],SIGNAL(readyRead()),this,SLOT(on_readyRead()));
    server[4]->pauseAccepting();
    if(socket[0]&&socket[1]&&socket[2]&&socket[3]&&socket[4]){
        emit clockIsOK();
    }
}
void Clock::on_readyRead()
{
    mutex.lock();
    stepIsOK++;
    if(stepIsOK==5)
    {
        y86Awake->wakeAll();
        stepIsOK=0;
    }
    mutex.unlock();
}
