#include "execute.h"
#include "const.h"
Execute::Execute()
{
    init();
}

void Execute::run()
{
    exec();
}

void Execute::init()
{
    socketForDecode=NULL;
    clientToMemory=NULL;
    clientToClock=NULL;
    serverForDecode=new QTcpServer();
    serverForDecode->listen(QHostAddress::Any,EXECUTE_FOR_DECODE_PORT);
    connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));
    serverForFetch=new QTcpServer();
    serverForFetch->listen(QHostAddress::Any,EXECUTE_FOR_FETCH_PORT);
    connect(serverForFetch,SIGNAL(newConnection()),this,SLOT(dealFetchConnection()));
}

Execute::~Execute()
{
    delete serverForDecode;
}

void Execute::move()
{
    moveToThread(this);
    serverForDecode->moveToThread(this);
    serverForFetch->moveToThread(this);
    socketForDecode->moveToThread(this);
    socketForFetch->moveToThread(this);//??????
    clientToMemory->moveToThread(this);
    clientToClock->moveToThread(this);
}
void Execute::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}

void Execute::dealFetchConnection()
{
    socketForFetch=serverForFetch->nextPendingConnection();
    serverForFetch->pauseAccepting();
}

//生成传到Fetch阶段的数据
QJsonObject Execute::dataToFetch()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(e_stat != 0)
        return sendData;
    sendData.insert("E_icode",E_icode);
    if(E_icode == 7)
        sendData.insert("e_Cnd",e_Cnd);
    if(E_icode == 5 || E_icode  == 11)
    {
        sendData.insert("E_dstM",E_dstM);
    }

    return sendData;
}

void Execute::sendToFetch(QJsonObject json)
{
    if(socketForFetch->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForFetch->write(bytes);
}

//生成传输到Decode阶段的数据
QJsonObject Execute::dataToDecode()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(e_stat != 0)
        return sendData;

    if(E_icode == 7)
    {
        sendData.insert("E_icode",E_icode);
        sendData.insert("e_Cnd",e_Cnd);
    }

    //加载，使用数据冒险
    if(E_icode == 5 || E_icode  == 11)
    {
        sendData.insert("E_icode",E_icode);
        sendData.insert("E_dstM",E_dstM);
    }

    //转发
    if(E_icode == 2 || E_icode == 3 || E_icode == 6
          || E_icode == 8 || E_icode == 9 || E_icode
            == 10 || E_icode == 11)
    {
        sendData.insert("E_icode",E_icode);
        sendData.insert("e_dstE",e_dstE);
        sendData.insert("e_valE",e_valE);
    }

    return sendData;
}

void Execute::sendToDecode(QJsonObject json)
{
    if(socketForDecode->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    socketForDecode->write(bytes);
}

//需要补写Execute 到 Decode阶段的数据传输，冒险处理
QJsonObject Execute::dataToMemory()
{
    QJsonObject sendData;
    if(isEnd)
        return sendData;
    if(e_stat != 0)
    {
        sendData.insert("M_stat",e_stat);
        if(e_stat)
            sendData.insert("instruction",instruction);
        return sendData;
    }
    sendData.insert("M_stat",e_stat);
    sendData.insert("M_icode",e_icode);
    sendData.insert("instruction",instruction);
    switch (e_icode) {
    case 1:
        return sendData;
    case 2:
    case 3:
    case 6:
        sendData.insert("M_dstE",e_dstE);
        sendData.insert("M_valE",e_valE);
        return sendData;
    case 5:
        sendData.insert("M_valE",e_valE);
        sendData.insert("M_dstM",e_dstM);
        return sendData;
    case 4:
        sendData.insert("M_valE",e_valE);
        sendData.insert("M_valA",e_valA);
        return sendData;
    case 8:
    case 9:
    case 10:
        sendData.insert("M_dstE",e_dstE);
        sendData.insert("M_valE",e_valE);
        sendData.insert("M_valA",e_valA);
        return sendData;
    case 11:
        sendData.insert("M_dstM",e_dstM);
        sendData.insert("M_dstE",e_dstE);
        sendData.insert("M_valE",e_valE);
        sendData.insert("M_valA",e_valA);
        return sendData;
    case 7:
        sendData.insert("M_valA",e_valA);
        sendData.insert("M_Cnd",e_Cnd);
        return sendData;
    }
    return sendData;
}

void Execute::sendToMemory(QJsonObject json)
{
    if(clientToMemory->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToMemory->write(bytes);
}

QJsonObject Execute::CCData()
{
    QJsonObject sendData;
    sendData.insert("SF",SF);
    sendData.insert("ZF",ZF);
    sendData.insert("OF",OF);
    return sendData;
}

void Execute::dealDecodeData()
{
    //get:E_stat,E_icode,E_ifun,E_valC,E_valA,E_valB,E_dstE,E_dstM;
    QByteArray bytes=socketForDecode->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    emit sendFromExecute(json);

    if(!json.contains("E_stat"))
    {
        isEnd = true;
        return;
    }
    E_stat = json.value("E_stat").toInt();
    if(E_stat == 1)
    {
        instruction = json.value("instruction").toString();
    }
    if(json.contains("E_icode"))
    {
        E_icode = json.value("E_icode").toInt();
        E_ifun = json.value("E_ifun").toInt();
        instruction = json.value("instruction").toString();
    }

    if(json.contains("E_valC"))
        E_valC = json.value("E_valC").toInt();
    if(json.contains("E_valA"))
        E_valA = json.value("E_valA").toInt();
    if(json.contains("E_valB"))
        E_valB = json.value("E_valB").toInt();
    if(json.contains("E_dstE"))
        E_dstE = json.value("E_dstE").toInt();
    if(json.contains("E_dstM"))
        E_dstM = json.value("E_dstM").toInt();
    if(json.contains("E_srcA"))
        E_srcA = json.value("E_srcA").toInt();
    if(json.contains("E_srcB"))
        E_srcB = json.value("E_srcB").toInt();
}

//预处理aluA的值
void Execute::ALU_A()
{
    switch (E_icode) {
    case 1:
        break;
    case 2:
        aluA = E_valA;
        break;
    case 3:
        aluA = E_valC;
        break;
    case 4:
        aluA = E_valC;
        break;
    case 5:
        aluA = E_valC;
        break;
    case 6:
        aluA = E_valA;
        break;
    case 8:
        aluA = -4;
        break;
    case 9:
        aluA = 4;
        break;
    case 10:
        aluA  = -4;
        break;
    case 11:
        aluA = 4;
        break;
    default:
        break;
    }
}

//预处理aluB的值
void Execute::ALU_B()
{
    switch (E_icode) {
    case 1:
        break;
    case 2:
    case 3:
        aluB = 0;
        break;
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
        aluB = E_valB;
        break;
    default:
        break;
    }
}

void Execute::setCC()
{
    if(e_valE == 0)
        ZF = 1;
    else ZF = 0;
    if(e_valE < 0)
        SF = 1;
    else SF = 0;
    if((aluA < 0 == aluB < 0) && (e_valE < 0 != aluA < 0))
        OF = 1;
    else OF = 0;
}

void Execute::execute()
{
    e_stat = E_stat;
    if(e_stat != 0)
        return;
    e_icode = E_icode;
    //e_dstM = E_dstM;
    //e_dstE = E_dstE;
    ALU_A();
    ALU_B();
    switch (e_icode) {
    case 1:
        break;
    case 4:
        e_valA = E_valA;
        e_valE = aluB + aluA;
        break;
    case 2:
    case 3:
        e_dstE = E_dstE;
        e_valE = aluB + aluA;
        break;
    case 5:
        e_dstM = E_dstM;
        e_valE = aluB + aluA;
        break;
    case 8:
    case 9:
    case 10:
        e_dstE = E_dstE;
        e_valA = E_valA;
        e_valE = aluB + aluA;
        break;
    case 11:
        e_dstE = E_dstE;
        e_dstM = E_dstM;
        e_valA = E_valA;
        e_valE = aluB + aluA;
        break;
    case 6:
        e_dstE = E_dstE;
        switch (E_ifun) {
        case 0:
            e_valE = aluB + aluA;
            break;
        case 1:
            e_valE = aluB - aluA;
            break;
        case 2:
            e_valE = aluB & aluA;
            break;
        case 3:
            e_valE = aluB ^ aluA;
            break;
        default:
            break;
        }
        setCC();
        break;
    case 7:
        e_valA = E_valA;
        switch (E_ifun) {
        case 0:
            e_Cnd = 1;
            break;
        case 1:
            if(SF || ZF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 2:
            if(SF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 3:
            if(ZF)
                e_Cnd =1;
            else e_Cnd = 0;
            break;
        case 4:
            if(!ZF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 5:
            if(!SF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 6:
            if(!SF && !ZF)
                e_Cnd = 1;
            else e_Cnd =0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
void Execute::circleBegin()
{
    qWarning()<<"execute Circle";
    QString str=QString(clientToClock->readAll());
    if(str=="nextStep")
    {
        if(!isEnd)
        {
            execute();
            emit sendCC(CCData());
        }
        //sleep(1);
        sendToMemory(dataToMemory());
        sendToDecode(dataToDecode());
        sendToFetch(dataToFetch());
    }else if(str=="restart")
    {
        E_stat = -1;
        e_Cnd = -1;
        ZF = 0;SF = 0;OF = 0;
        isEnd = false;

        QJsonObject json;
        emit sendFromExecute(json);
        emit sendCC(json);
    }
    clientToClock->write("done");
    clientToClock->waitForBytesWritten();
    qDebug()<<"executedone";
}

