#include "writeback.h"
#include "const.h"
#include "globle.h"
#include "mainwindow.h"
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
    //get:W_stat,W_icode,W_valE,W_valM,W_dstE,W_dstM;
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

//给寄存器写回相应的值
void Writeback::writeReg(int dst, int val)
{
    switch (dst) {
    case 0:
        globle::eax = val;
        break;
    case 1:
        globle::ecx = val;
        break;
    case 2:
        globle::ebx = val;
        break;
    case 3:
        globle::edx = val;
        break;
    case 4:
        globle::esp = val;
        break;
    case 5:
        globle::ebp = val;
        break;
    case 6:
        globle::esi = val;
        break;
    case 7:
        globle::edi = val;
        break;
    default:
        break;
    }
}

void Writeback::writeback()
{
    if(W_stat != 0)
        return;
    switch (W_icode) {
    case 1:
        break;
    case 2:
        writeReg(W_dstE,W_valE);
        break;
    case 3:
        writeReg(W_dstE,W_valE);
        break;
    case 5:
        writeReg(W_dstM,W_valM);
        break;
    case 6:
        writeReg(W_dstE,W_valE);
        break;
    case 8:
        writeRed(W_dstE,W_valE);
        break;
    case 9:
        writeReg(W_dstE,W_valE);
        break;
    case 10:
        writeReg(W_dstE,W_valE);
        break;
    case 11:
        writeReg(W_dstE,W_valE);
        writeReg(W_dstM,W_valM);
        break;
    default:
        break;
    }
}
