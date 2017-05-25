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
    //get:M_stat,M_icode,M_Cnd,M_valE,M_valA,M_dstE,M_dstM;
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

void Memory::memory()
{
    m_stat = M_stat;
    if(m_stat != 0)
        return;
    m_icode = M_icode;
    m_dstE = M_dstE;
    m_dstM = M_dstM;
    switch (m_icode) {
    case 1:
        break;
    case 4:
        mem[M_valE] = M_valA;
        break;
    case 5:
        if(mem.contains(M_valE))
            m_valM = mem[M_valE];
        else
            m_stat = 2;
        break;
    case 8:
        mem[M_valE] = M_valA;
        break;
    case 9:
        if(mem.contains(M_valA))
            m_valM = mem[M_valA];
        else
            m_stat = 2;
        break;
    default:
        break;
    }

}
