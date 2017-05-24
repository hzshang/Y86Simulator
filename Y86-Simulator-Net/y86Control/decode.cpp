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
    clientToClock=NULL;
    serverForFetch=new QTcpServer();
    serverForFetch->listen(QHostAddress::Any,DECODE_FOR_FETCH_PORT);
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}

void Decode::sendToExecute(QJsonObject json)
{
    if(clientToExecute->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToExecute->write(bytes);
}

void Decode::sendToMemory(QJsonObject json)
{
    if(clientToMemory->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToMemory->write(bytes);
}

void Decode::sendToWriteback(QJsonObject json)
{
    if(clientToWriteback->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToWriteback->write(bytes);
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
    //get: D_stat,D_icode,D_rA,D_rB,D_valC,D_valP;
}

void Decode::dealExecuteData()
{
   //get: e_desE,e_valE,
}

void Decode::dealMemoryData()
{
   //get: M_desM,m_valM,M_desE,M_valE
}

void Decode::dealWritebackData()
{
   //get: W_dstM,W_valM,W_dstE,W_valE
}
//读取寄存器的值
int Decode::getRegValue(int src)
{
    switch(src)
      {
         case 0:return eax;
         case 1:return ecx;
         case 2:return edx;
         case 3:return ebx;
         case 4:return esp;
         case 5:return ebp;
         case 6:return esi;
         case 7:return edi;
      }
}

void Decode::decode()
{
    //分支错误，冒险处理等
    d_stat = D_stat;
    d_icode = D_icode;
    d_ifun = D_ifun;
    d_valC = D_valC;

    if(!d_stat)
    {
        switch (d_icode) {
        case 1:
            break;
        case 2:
            d_srcA = D_rA;
            d_dstE = D_rB;
            break;
        case 3:
            d_dstE = D_rB;
            break;
        case 4:
            d_srcA = D_rA;
            d_srcB = D_rB;
            break;
        case 5:
            d_srcB = D_rB;
            d_dstM = D_rA;
            break;
        case 6:
            d_srcA = D_rA;
            d_srcB = D_rB;
            d_dstE = D_rB;
            break;
        case 8:
            d_srcB = 4;
            d_dstE = 4;
            break;
        case 9:
            d_srcA = 4;
            d_srcB = 4;
            d_dstE = 4;
            break;
        case 10:
            d_srcA = D_rA;
            d_srcB = 4;
            d_dstE = 4;
            break;
        case 11:
            d_srcA = 4;
            d_srcB = 4;
            d_dstE = 4;
            d_dstM = D_rA;
        default:
            break;
        }
    }
    //加载使用数据冒险处理
}

//d_valA的转发器和选择器
void Decode::sel_fwd_valA()
{
    if(d_icode == 7 || d_icode == 8)
    {
        d_valA = D_valP;
        return;
    }
    else if(d_srcA == e_dstE)
    {
        d_valA = e_valE;
        return;
    }
    else if(d_srcA == M_dstM)
    {
        d_valA = m_valM;
        return;
    }
    else if(d_srcA == M_dstE)
    {
        d_valA = M_valE;
        return;
    }
    else if(d_srcA == W_dstM)
    {
        d_valA = W_valM;
        return;
    }
    else if(d_srcA == W_dstE)
    {
        d_valA = W_valE;
        return;
    }
    else
    {
        d_valA = getRegValue(d_srcA);
        return;
    }
}

//d_valB的转发器
void Decode::fwd_valB()
{
    if(d_srcB == e_dstE)
    {
        d_valB = e_valE;
        return;
    }
    else if(d_srcB == M_dstM)
    {
        d_valB = m_valM;
        return;
    }
    else if(d_srcB == M_dstE)
    {
        d_valB = M_valE;
        return;
    }
    else if(d_srcB == W_dstM)
    {
        d_valB = W_valM;
        return;
    }
    else if(d_srcB == W_dstE)
    {
        d_valB = W_valE;
        return;
    }
    else
    {
        d_valB = getRegValue(d_srcB);
        return;
    }
}
