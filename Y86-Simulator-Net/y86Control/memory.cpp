#include "memory.h"
#include "const.h"
Memory::Memory()
{
    init();
}

Memory::~Memory()
{
    delete serverForExecute;
    delete serverForDecode;
    delete serverForFetch;

}
void Memory:: dealExecuteConnection()
{
    socketForExecute=serverForExecute->nextPendingConnection();
    connect(socketForExecute,SIGNAL(readyRead()),this,SLOT(dealExecuteData()));
    serverForExecute->pauseAccepting();
}

void  Memory::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}

void  Memory:: dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    connect(socketForFetch,SIGNAL(readyRead()),this,SLOT(dealFetchData()));
    serverForFetch->pauseAccepting();
}

void  Memory::dealExecuteData()
{

}

void  Memory::dealDecodeData()
{

}

void  Memory::dealFetchData()
{

}

void  Memory::dealWritebackData()
{

}

void Memory::init()
{
    socketForExecute=NULL;
    socketForDecode=NULL;
    socketForFetch=NULL;
    clientToWriteback=NULL;

    serverForExecute=new QTcpServer();
    serverForExecute->listen(QHostAddress::Any,MEMORY_FOR_EXECUTE_PORT);
    connect(serverForExecute,SIGNAL(newConnection()),this,SLOT(dealExecuteConnection()));

    serverForDecode=new QTcpServer();
    serverForDecode->listen(QHostAddress::Any,MEMORY_FOR_DECODE_PORT);
    connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));

    serverForFetch=new QTcpServer();
    serverForFetch->listen(QHostAddress::Any,MEMORY_FOR_FETCH_PORT);
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}
