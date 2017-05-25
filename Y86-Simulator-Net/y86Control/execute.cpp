#include "execute.h"
#include "const.h"
Execute::Execute()
{
    init();
}

void Execute::init()
{
    socketForDecode=NULL;
    clientToMemory=NULL;
    clientToClock=NULL;
    serverForDecode=new QTcpServer();
    serverForDecode->listen(QHostAddress::Any,EXECUTE_FOR_DECODE_PORT);
    connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));
    serverForFetch=new QTcpServer();
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}

void Execute::sendToMemory(QJsonObject json)
{
    if(clientToMemory->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToMemory->write(bytes);
}

void Execute::sendToFetch(QJsonObject json)
{
    if(clientToFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToFetch->write(bytes);
}
Execute::~Execute()
{
    delete serverForDecode;
}
void Execute::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}

void Execute::dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
//    connect(socketForFetch,SIGNAL(readyRead()),this,SLOT(dealFetchData()));
    serverForFetch->pauseAccepting();
}
void Execute::sendToFetch(QJsonObject json)
{
    if(socketForFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForFetch->write(bytes);
}

void Execute::dealDecodeData()
{

}

void Execute::dealMemoryData()
{

}
