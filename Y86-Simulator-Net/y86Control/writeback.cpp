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

void Writeback::move()
{
    moveToThread(this);
    serverForFetch->moveToThread(this);
    socketForFetch->moveToThread(this);

    serverForDecode->moveToThread(this);
    socketForDecode->moveToThread(this);

    serverForMemory->moveToThread(this);
    socketForMemory->moveToThread(this);
    clientToClock->moveToThread(this);
}

void Writeback::run()
{
    exec();
}
void Writeback::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
//    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}
void Writeback::dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    serverForFetch->pauseAccepting();
}

void Writeback::dealMemoryConnection()
{
    socketForMemory=serverForMemory->nextPendingConnection();
    connect(socketForMemory,SIGNAL(readyRead()),this,SLOT(dealMemoryData()));
    serverForMemory->pauseAccepting();
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
     clientToClock=NULL;
}

//生成传输到decode的数据
QJsonObject Writeback::dataToDecode()
{
    //get: W_dstM,W_valM,W_dstE,W_valE
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(W_stat != 0)
        return sendData;
    switch (W_icode) {
    case 2:
    case 3:
    case 6:
    case 8:
    case 9:
    case 10:
        sendData.insert("W_dstE",W_dstE);
        sendData.insert("W_valE",W_valE);
        return sendData;
    case 5:
        sendData.insert("W_dstM",W_valM);
        sendData.insert("W_valM",W_valM);
        return sendData;
    case 11:
        sendData.insert("W_dstE",W_dstE);
        sendData.insert("W_valE",W_valE);
        sendData.insert("W_dstM",W_valM);
        sendData.insert("W_valM",W_valM);
        return sendData;
    default:
        return sendData;
    }
    return sendData;
}

void Writeback::sendToDecode(QJsonObject json)
{
    if(socketForDecode->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForDecode->write(bytes);
}

//生成传输到fetch的数据
QJsonObject Writeback::dataToFetch()
{
    //get:W_icode,W_valM
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(W_stat != 0)
        return sendData;
    if(W_icode == 9)
    {
        sendData.insert("W_icode",W_icode);
        sendData.insert("W_valM",W_valM);
    }
    return sendData;
}

void Writeback::sendToFetch(QJsonObject json)
{
    if(socketForFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForFetch->write(bytes);
}

void Writeback::dealMemoryData()
{
    //get:W_stat,W_icode,W_valE,W_valM,W_dstE,W_dstM;
    QByteArray bytes=socketForMemory->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    emit sendFromWriteback(json);

    if(!json.contains("W_stat"))
    {
        isEnd = true;
        return;
    }
    W_stat = json.value("W_stat").toInt();
    if(W_stat == 1)
    {
        instruction = json.value("instruction").toString();
    }
    if(json.contains("W_icode"))
    {
        W_icode = json.value("W_icode").toInt();
        instruction = json.value("instruction").toString();
    }
    if(json.contains("W_valE"))
        W_valE = json.value("W_valE").toInt();
    if(json.contains("W_valM"))
        W_valM = json.value("W_valM").toInt();
    if(json.contains("W_dstE"))
        W_dstE = json.value("W_dstE").toInt();
    if(json.contains("W_dstM"))
        W_dstM = json.value("W_dstM").toInt();
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
        globle::edx = val;
        break;
    case 3:
        globle::ebx = val;
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
        writeReg(W_dstE,W_valE);
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

void Writeback::circleBegin()
{
    qWarning()<<"writeback Circle";
    QString str=QString(clientToClock->readAll());
    if(str=="nextStep")
    {
        if(!isEnd)
        {
            writeback();
        }
        sendToDecode(dataToDecode());
        sendToFetch(dataToFetch());
    }else if(str=="restart")
    {
        W_stat = -1;
        isEnd = false;
        globle::eax = 0;
        globle::ebx = 0;
        globle::ecx = 0;
        globle::edx = 0;
        globle::esp = 0;
        globle::ebp = 0;
        globle::esi = 0;
        globle::edi = 0;

        QJsonObject json;
        emit sendFromWriteback(json);
        emit clearReg(true);
    }
    clientToClock->write("done");
    clientToClock->waitForBytesWritten();
    qDebug()<<"writedone";
}
