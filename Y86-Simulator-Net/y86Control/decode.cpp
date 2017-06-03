#include "decode.h"
#include "const.h"
#include "globle.h"
#include "mainwindow.h"

Decode::Decode()
{
    init();
}

void Decode::run()
{
    exec();
}

void Decode::init()
{
    socketForFetch=NULL;
    clientToExecute=NULL;
    clientToMemory=NULL;
    clientToWriteback=NULL;
    clientToClock=NULL;
    doneNum=0;
    serverForFetch=new QTcpServer();
    serverForFetch->listen(QHostAddress::Any,DECODE_FOR_FETCH_PORT);
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}

Decode::~Decode()
{
    delete serverForFetch;
}

void Decode::move()
{
    moveToThread(this);
    serverForFetch->moveToThread(this);
    socketForFetch->moveToThread(this);
    clientToExecute->moveToThread(this);
    clientToMemory->moveToThread(this);
    clientToWriteback->moveToThread(this);
    clientToClock->moveToThread(this);
}

//生成传递给Execute阶段的数据
QJsonObject Decode::dataToExecute()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(d_stat != 0)
    {
        sendData.insert("E_stat",d_stat);
        return sendData;
    }
    sendData.insert("E_stat",d_stat);
    sendData.insert("E_icode",d_icode);
    sendData.insert("E_ifun",d_ifun);
    sendData.insert("instruction",instruction);
    switch (d_icode) {
    case 1:
        return sendData;
    case 2:
        sendData.insert("E_srcA",d_srcA);
        sendData.insert("E_valA",d_valA);
        sendData.insert("E_dstE",d_dstE);
        return sendData;
    case 3:
        sendData.insert("E_dstE",d_dstE);
        sendData.insert("E_valC",d_valC);
        return sendData;
    case 4:
        sendData.insert("E_srcA",d_srcA);
        sendData.insert("E_valA",d_valA);
        sendData.insert("E_srcB",d_srcB);
        sendData.insert("E_valB",d_valB);
        sendData.insert("E_valC",d_valC);
        return sendData;
    case 5:
        sendData.insert("E_srcB",d_srcB);
        sendData.insert("E_valB",d_valB);
        sendData.insert("E_dstM",d_dstM);
        sendData.insert("E_valC",d_valC);
        return sendData;
    case 6:
    case 9:
    case 10:
        sendData.insert("E_srcA",d_srcA);
        sendData.insert("E_valA",d_valA);
        sendData.insert("E_srcB",d_srcB);
        sendData.insert("E_valB",d_valB);
        sendData.insert("E_dstE",d_dstE);
        return sendData;
    case 7:
        sendData.insert("E_valA",d_valA);
        return sendData;
    case 8:
        sendData.insert("E_srcB",d_srcB);
        sendData.insert("E_valB",d_valB);
        sendData.insert("E_dstE",d_dstE);
        sendData.insert("E_valA",d_valA);
        return sendData;
    case 11:
        sendData.insert("E_srcA",d_srcA);
        sendData.insert("E_valA",d_valA);
        sendData.insert("E_srcB",d_srcB);
        sendData.insert("E_valB",d_valB);
        sendData.insert("E_dstE",d_dstE);
        sendData.insert("E_dstM",d_dstM);
        return sendData;
    }
    return sendData;
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

//生成传输到fetch阶段的数据，加载使用数据冒险处理
QJsonObject Decode::dataToFetch()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(d_stat != 0)
        return sendData;
    switch (d_icode) {
    case 2:
        sendData.insert("d_srcA",d_srcA);
        return sendData;
    case 4:
    case 6:
    case 9:
    case 10:
    case 11:
        sendData.insert("d_srcA",d_srcA);
        sendData.insert("d_srcB",d_srcB);
        return sendData;
    case 5:
    case 8:
        sendData.insert("d_srcB",d_srcB);
        return sendData;
    default:
        return sendData;
    }
    return sendData;
}

void Decode::sendToFetch(QJsonObject json)
{
    if(socketForFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForFetch->write(bytes);
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
    QByteArray bytes=socketForFetch->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    emit sendFromDecode(json);
    if(!json.contains("D_stat"))
    {
        isEnd = true;
        return;
    }
    D_stat = json.value("D_stat").toInt();
    if(json.contains("D_icode"))
    {
        D_icode = json.value("D_icode").toInt();
        D_ifun = json.value("D_ifun").toInt();
        D_valP = json.value("D_valP").toInt();
        instruction = json.value("instruction").toString();
    }

    if(json.contains("D_rA"))
        D_rA = json.value("D_rA").toInt();
    if(json.contains("D_rB"))
        D_rB = json.value("D_rB").toInt();
    if(json.contains("D_valC"))
        D_valC = json.value("D_valC").toInt();
}

void Decode::dealExecuteData()
{
   //get: e_desE,e_valE,E_icode,e_Cnd;
    QByteArray bytes=clientToExecute->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("E_icode"))
        E_icode = json.value("E_icode").toInt();
    else
        E_icode = -1;

    if(json.contains("e_Cnd"))
        e_Cnd = json.value("e_Cnd").toInt();
    if(json.contains("E_dstM"))
        E_dstM = json.value("E_dstM").toInt();
    if(json.contains("e_dstE"))
    {
        e_dstE = json.value("e_dstE").toInt();
        e_valE = json.value("e_valE").toInt();
    }
    mutexNum.lock();
    doneNum++;
    if(doneNum==3)
    {
        doneNum=0;
        wait.wakeAll();
    }
    mutexNum.unlock();
}

void Decode::dealMemoryData()
{
   //get: M_desM,m_valM,M_desE,M_valE
    QByteArray bytes=clientToMemory->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("M_dstM"))
    {
        M_dstM = json.value("M_dstM").toInt();
        m_valM = json.value("m_valM").toInt();
    }
    else
        M_dstM = -1;

    if(json.contains("M_dstE"))
    {
        M_dstE = json.value("M_dstE").toInt();
        M_valE = json.value("M_valE").toInt();
    }
    else
        M_dstE = -1;

    mutexNum.lock();
    doneNum++;
    if(doneNum==3)
    {
        doneNum=0;
        wait.wakeAll();
    }
    mutexNum.unlock();
}

void Decode::dealWritebackData()
{
   //get: W_dstM,W_valM,W_dstE,W_valE
    QByteArray bytes=clientToWriteback->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("W_dstM"))
    {
        W_dstM = json.value("W_dstM").toInt();
        W_valM = json.value("W_valM").toInt();
    }
    else
        W_dstM = -1;

    if(json.contains("W_dstE"))
    {
        W_dstE = json.value("W_dstE").toInt();
        W_valE = json.value("W_valE").toInt();
    }
    else
        W_dstE = -1;
    mutexNum.lock();
    doneNum++;
    if(doneNum==3)
    {
        doneNum=0;
        wait.wakeAll();
    }
    mutexNum.unlock();
}

//读取寄存器的值
int Decode::getRegValue(int src)
{
    switch(src)
      {
         case 0:return globle::eax;
         case 1:return globle::ecx;
         case 2:return globle::edx;
         case 3:return globle::ebx;
         case 4:return globle::esp;
         case 5:return globle::ebp;
         case 6:return globle::esi;
         case 7:return globle::edi;
      }
    return -1;
}

void Decode::decode()
{
    //分支错误
    if(E_icode == 7 && !e_Cnd)
        d_icode = 1;
    //加载，使用数据冒险处理
    else if(isRisk)
    {
        d_icode = icodeStorage;
        isRisk = false;
    }
    else
    {
        d_icode = D_icode;
    }
    d_stat = D_stat;
    d_ifun = D_ifun;
    //d_valC = D_valC;

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
            d_valC = D_valC;
            break;
        case 4:
            d_srcA = D_rA;
            d_srcB = D_rB;
            d_valC = D_valC;
            break;
        case 5:
            d_srcB = D_rB;
            d_dstM = D_rA;
            d_valC = D_valC;
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
    if((E_icode == 5 || E_icode == 11) &&
            (E_dstM == d_srcA || E_dstM == d_srcB))
    {
        icodeStorage = d_icode;
        d_icode = 1;
        isRisk = true;

    }
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

void Decode::circleBegin()
{
    qWarning()<<"decode Circle";
    QString str=QString(clientToClock->readAll());
    if(str=="nextStep")
    {
        //clientToWriteback->waitForReadyRead();
        //clientToMemory->waitForReadyRead();
        //clientToExecute->waitForReadyRead();
        mutexWait.lock();
        wait.wait(&mutexNum);
        mutexWait.unlock();
        if(!isEnd)
        {
            decode();
            sel_fwd_valA();
            fwd_valB();
        }
        sendToExecute(dataToExecute());
        sendToFetch(dataToFetch());
        qDebug()<<"decodedone";
    }else if(str=="restart")
    {
        D_stat = -1;
        E_icode = -1;
        M_dstE = -1;
        M_dstM = -1;
        W_dstE = -1;
        W_dstM = -1;

        isRisk = false;
        isEnd = false;

        QJsonObject json;
        emit sendFromDecode(json);
    }
    clientToClock->write("done");
    clientToClock->waitForBytesWritten();

}
