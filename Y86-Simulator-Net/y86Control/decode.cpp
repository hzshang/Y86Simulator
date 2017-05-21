#include "decode.h"
#include "const.h"
Decode::Decode()
{
    init();
}

void Decode::init()
{
    socketForFetch=NULL;
    clientToExecute=NULL;
    clientToMemory=NULL;
    clientToWriteback=NULL;
    serverForFetch=new QTcpServer();
    serverForFetch->listen(QHostAddress::Any,DECODE_FOR_FETCH_PORT);
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}

Decode::~Decode()
{
    delete serverForFetch;

}
void Decode::dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    connect(socketForFetch,SIGNAL(readyRead()),this,SLOT(dealFetchData()));
    serverForFetch->pauseAccepting();
}

void Decode::dealFetchData()
{

}

void Decode::dealExecuteData()
{

}

void Decode::dealMemoryData()
{

}

void Decode::dealWritebackData()
{

}
