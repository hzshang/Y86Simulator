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

void Memory::move()
{
    moveToThread(this);
    serverForExecute->moveToThread(this);
    serverForDecode->moveToThread(this);
    serverForFetch->moveToThread(this);
    socketForExecute->moveToThread(this);
    socketForDecode->moveToThread(this);
    socketForFetch->moveToThread(this);
    clientToWriteback->moveToThread(this);
    clientToClock->moveToThread(this);
}

void Memory::run()
{
    exec();
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
    serverForDecode->pauseAccepting();
}

void  Memory:: dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    serverForFetch->pauseAccepting();
}

void Memory::init()
{
    socketForExecute=NULL;
    socketForDecode=NULL;
    socketForFetch=NULL;
    clientToWriteback=NULL;
    clientToClock=NULL;
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

//生成传到writeback的数据
QJsonObject Memory::dataToWriteback()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(m_stat != 0)
    {
        sendData.insert("W_stat",m_stat);
        if(m_stat == 1)
            sendData.insert("instruction",instruction);
        return sendData;
    }
    sendData.insert("W_stat",m_stat);
    sendData.insert("W_icode",m_icode);
    sendData.insert("instruction",instruction);
    switch (m_icode) {
    case 1:
    case 4:
    case 7:
        return sendData;
    case 2:
    case 3:
    case 6:
    case 8:
    case 10:
        sendData.insert("W_dstE",m_dstE);
        sendData.insert("W_valE",m_valE);
        return sendData;
    case 5:
        sendData.insert("W_dstM",m_dstM);
        sendData.insert("W_valM",m_valM);
        return sendData;
    case 9:
        sendData.insert("W_dstE",m_dstE);
        sendData.insert("W_valE",m_valE);
        sendData.insert("W_valM",m_valM);
        return sendData;
    case 11:
        sendData.insert("W_dstE",m_dstE);
        sendData.insert("W_dstM",m_dstM);
        sendData.insert("W_valE",m_valE);
        sendData.insert("W_valM",m_valM);
        return sendData;
    }
    return sendData;
}

void Memory::sendToWriteback(QJsonObject json)
{
    if(clientToWriteback->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToWriteback->write(bytes);
}

//生成传输到Fetch的数据
QJsonObject Memory::dataToFetch()
{
    //get:M_icode,M_Cnd,M_valA;
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(m_stat != 0)
        return sendData;
    if(M_icode == 7)
    {
        sendData.insert("M_icode",M_icode);
        sendData.insert("M_Cnd",M_Cnd);
        sendData.insert("M_valA",M_valA);
    }
    //if(M_icode == 4 || M_icode == 8 || M_icode == 9
            //|| M_icode == 10 || M_icode == 11)
        //sendData.insert("M_valA",M_valA);
    return sendData;
}

void Memory::sendToFetch(QJsonObject json)
{
    if(socketForFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForFetch->write(bytes);
}

//生成传输到Decode的数据
QJsonObject Memory::dataToDecode()
{
    //get: M_dstM,m_valM,M_dstE,M_valE
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(m_stat != 0)
        return sendData;
    switch (m_icode) {
    case 2:
    case 3:
    case 6:
    case 8:
    case 10:
       sendData.insert("M_dstE",M_dstE);
       sendData.insert("M_valE",M_valE);
       return sendData;
    //case 4:
        //sendData.insert("M_valE",M_valE);
        //return sendData;
    case 5:
        sendData.insert("M_dstM",M_dstM);
        //sendData.insert("M_valE",M_valE);
        sendData.insert("m_valM",m_valM);
        return sendData;
    case 9:
        sendData.insert("M_dstE",M_dstE);
        sendData.insert("M_valE",M_valE);
        //sendData.insert("m_valM",m_valM);
        return sendData;
    case 11:
        sendData.insert("M_dstM",M_dstM);
        sendData.insert("m_valM",m_valM);
        sendData.insert("M_dstE",M_dstE);
        sendData.insert("M_valE",M_valE);
        return sendData;
    default:
        return sendData;
    }
    return sendData;
}

void Memory::sendToDecode(QJsonObject json)
{
    if(socketForDecode->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForDecode->write(bytes);
}

void  Memory::dealExecuteData()
{
    //get:M_stat,M_icode,M_Cnd,M_valE,M_valA,M_dstE,M_dstM;
    QByteArray bytes=socketForExecute->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    emit sendFromMemory(json);
    if(!json.contains("M_stat"))
    {
        isEnd = true;
        return;
    }

    M_stat = json.value("M_stat").toInt();
    if(M_stat == 1)
    {
        instruction = json.value("instruction").toString();
    }
    if(json.contains("M_icode"))
    {
        M_icode = json.value("M_icode").toInt();
        instruction = json.value("instruction").toString();
    }
    if(json.contains("M_Cnd"))
        M_Cnd = json.value("M_Cnd").toInt();
    if(json.contains("M_valE"))
        M_valE = json.value("M_valE").toInt();
    if(json.contains("M_valA"))
        M_valA = json.value("M_valA").toInt();
    if(json.contains("M_dstE"))
        M_dstE = json.value("M_dstE").toInt();
    if(json.contains("M_dstM"))
        M_dstM = json.value("M_dstM").toInt();
}

void Memory::memory()
{
    m_stat = M_stat;
    if(m_stat != 0)
        return;
    m_icode = M_icode;
    switch (m_icode) {
    case 1:
        break;
    case 2:
    case 3:
    case 6:
        m_dstE = M_dstE;
        m_valE = M_valE;
    case 4:
        mem[M_valE] = M_valA;
        qDebug()<<M_valE;
        break;
    case 5:
        m_dstM = M_dstM;
        qDebug()<<M_valE;
        if(mem.contains(M_valE))
            m_valM = mem[M_valE];
        else
            m_stat = 2;
        break;
    case 8:
        m_dstE = M_dstE;
        m_valE = M_valE;
        mem[M_valE] = M_valA;
        break;
    case 9:
        m_dstE = M_dstE;
        m_valE = M_valE;
        if(mem.contains(M_valA))
            m_valM = mem[M_valA];
        else
            m_stat = 2;
        break;
    case 10:
        m_dstE = M_dstE;
        m_valE = M_valE;
        mem[M_valE] = M_valA;
        break;
    case 11:
        m_dstE = M_dstE;
        m_dstE = M_dstM;
        m_valE = M_valE;
        if(mem.contains(M_valA))
            m_valM = mem[M_valA];
        else
            m_stat = 2;
        break;
    default:
        break;
    }
}

void Memory::circleBegin()
{
    qWarning()<<"memory Circle";
    QString str=QString(clientToClock->readAll());
    if(str=="nextStep")
    {
        if(!isEnd)
        {
            memory();
        }
        sendToWriteback(dataToWriteback());
        sendToDecode(dataToDecode());
        sendToFetch(dataToFetch());
    }else if(str=="restart")
    {
        M_stat = -1;
        isEnd = false;
        mem.clear();
        QJsonObject json;
        emit sendFromMemory(json);
    }
    clientToClock->write("done");
    clientToClock->waitForBytesWritten();
    qDebug()<<"memorydone";
}

