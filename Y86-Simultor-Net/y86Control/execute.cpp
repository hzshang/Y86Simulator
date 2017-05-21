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
    serverForDecode=new QTcpServer();
    serverForDecode->listen(QHostAddress::Any,EXECUTE_FOR_DECODE_PORT);
    connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));
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

void Execute::dealDecodeData()
{

}

void Execute::dealMemoryData()
{

}
