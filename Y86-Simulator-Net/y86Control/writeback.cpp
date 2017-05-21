#include "writeback.h"
#include "const.h"
Writeback::Writeback()
{
    init();
}

Writeback::~Writeback()
{
    delete serverForFetch;
    delete serverForDecode;
    delete serverForMemory;
}
void Writeback::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}
void Writeback::dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    connect(socketForFetch,SIGNAL(readyRead()),this,SLOT(dealFetchData()));
    serverForFetch->pauseAccepting();
}

void Writeback::dealMemoryConnection()
{
    socketForMemory=serverForMemory->nextPendingConnection();
    connect(socketForMemory,SIGNAL(readyRead()),this,SLOT(dealMemoryData()));
    serverForMemory->pauseAccepting();
}
void Writeback::dealMemoryData()
{

}

void Writeback::init()
{
     socketForFetch=NULL;
     socketForDecode=NULL;
     socketForMemory=NULL;

     serverForFetch=new QTcpServer();
     serverForFetch->listen(QHostAddress::Any,WRITEBACK_FOR_FETCH_PORT);
     connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));

     serverForDecode=new QTcpServer();
     serverForDecode->listen(QHostAddress::Any,WRITEBACK_FOR_DECODE_PORT);
     connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));

     serverForMemory=new QTcpServer();
     serverForMemory->listen(QHostAddress::Any,WRITEBACK_FOR_MEMORY_PORT);
     connect(serverForMemory,SIGNAL(newConnection()),this,SLOT(dealMemoryConnection()));

}
void Writeback::dealFetchData()
{

}
void Writeback::dealDecodeData()
{

}
