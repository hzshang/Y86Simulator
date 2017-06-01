#include "y86.h"
#include <QMetaType>
Y86::Y86()
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    listen=new QUdpSocket();
    broadcast=new QUdpSocket();
    broadcast->moveToThread(this);
    circleTime=50;
    master=false;
}
//发射广播寻找局域网客户端，建立连接
void Y86::ready(bool FLevel, bool DLevel, bool ELevel, bool MLevel, bool WLevel)
{
    init();
    if(FLevel)
    {
        fetch=new Fetch();
        master=true;
        clock=new Clock(&awake);
        clock->ready();
        connect(clock,SIGNAL(clockIsOK()),this,SLOT(on_clockIsOK()));
        connect(this,SIGNAL(nextStep()),clock,SLOT(nextStep()));
    }
    if(DLevel)
        decode=new Decode();
    if(ELevel)
        execute=new Execute();
    if(MLevel)
        memory=new Memory();
    if(WLevel)
        writeback=new Writeback();
    newThreadBroad();
}

//Y86广播收到信号
void Y86::readFromlisten()
{
    while(listen->state()==QUdpSocket::BoundState&&listen->hasPendingDatagrams())
    {
        QByteArray datagram;
        QHostAddress address;
        datagram.resize(listen->pendingDatagramSize());
        listen->readDatagram(datagram.data(),datagram.size(),&address);
        QJsonObject json=QJsonDocument::fromBinaryData(datagram).object();
        emit on_dealData(json,address);
    }
}
//广播信息接口
void Y86::broadData(QJsonObject &data)
{
    QByteArray bytes=QJsonDocument(data).toBinaryData();
    broadcast->writeDatagram(bytes,bytes.size(),QHostAddress::Broadcast,Y86PORT);
}
//处理收到的信息
void Y86::dealData(QJsonObject json,QHostAddress address)
{
    switch(json["id"].toInt())
    {
    case 0://暂时不能实现
        break;
    case 1:
        beignConnect(json,address);
        if(master)
            countConnection(json["pool"].toInt());
        break;
    case 3:
        beginPipeLine();
        break;
    }
}

void Y86::on_clockIsOK()
{
    clockIsOk=true;
    if(pool==0x000001ff)
    {
        QJsonObject json;
        json.insert("id",3);
        broadData(json);
    }
}

void Y86::initPipeline()
{

}

void Y86::beginPipeLine()
{
    stopBroadcast=true;
    listen->readAll();
    listen->close();
    //分出5个线程
    if(fetch)
    {
        clock->move();
        clock->start();
        fetch->move();
        fetch->start();
    }
    if(decode)
    {
        decode->move();
        decode->start();
    }
    if(execute)
    {
        execute->move();
        execute->start();
    }
    if(memory)
    {
        memory->move();
        memory->start();
    }
    if(writeback)
    {
        writeback->move();
        writeback->start();
    }
    emit showPipeLine(this);
}
void Y86::beignConnect(QJsonObject json,QHostAddress address)
{
    if(json["FLevel"].toBool())
    {
        if(fetch && !fetch->clientToClock)
        {
            fetch->clientToClock=new QTcpSocket();
            fetch->clientToClock->connectToHost(address,CLOCK_FOR_FETCH);
            if(!fetch->clientToClock->waitForConnected())
            {
                delete fetch->clientToClock;
                fetch->clientToClock=NULL;
            }else{
                connect(fetch->clientToClock,SIGNAL(readyRead()),fetch,SLOT(circleBegin()),Qt::QueuedConnection);
            }
        }
        if(decode&& !decode->clientToClock)
        {
            decode->clientToClock=new QTcpSocket();
            decode->clientToClock->connectToHost(address,CLOCK_FOR_Decode);
            if(!decode->clientToClock->waitForConnected())
            {
                delete decode->clientToClock;
                decode->clientToClock=NULL;
            }else{
                connect(decode->clientToClock,SIGNAL(readyRead()),decode,SLOT(circleBegin()),Qt::QueuedConnection);
            }
        }
        if(execute&& !execute->clientToClock)
         {
            execute->clientToClock=new QTcpSocket();
            execute->clientToClock->connectToHost(address,CLOCK_FOR_Execute);
            if(!execute->clientToClock->waitForConnected())
            {
                delete execute->clientToClock;
                execute->clientToClock=NULL;
            }else{
                connect(execute->clientToClock,SIGNAL(readyRead()),execute,SLOT(circleBegin()),Qt::QueuedConnection);
            }
        }
        if(memory&& !memory->clientToClock)
        {
            memory->clientToClock=new QTcpSocket();
            memory->clientToClock->connectToHost(address,CLOCK_FOR_Memory);
            if(!memory->clientToClock->waitForConnected())
            {
                delete memory->clientToClock;
                memory->clientToClock=NULL;
            }else{
                connect(memory->clientToClock,SIGNAL(readyRead()),memory,SLOT(circleBegin()),Qt::QueuedConnection);
            }
        }
        if(writeback&& !writeback->clientToClock)
        {
            writeback->clientToClock=new QTcpSocket();
            writeback->clientToClock->connectToHost(address,CLOCK_FOR_Writeabck);
            if(!writeback->clientToClock->waitForConnected())
            {
                delete writeback->clientToClock;
                writeback->clientToClock=NULL;
            }else{
                connect(writeback->clientToClock,SIGNAL(readyRead()),writeback,SLOT(circleBegin()),Qt::QueuedConnection);
            }
        }
    }
    if(json["DLevel"].toBool())
    {
        if(fetch && !fetch->clientToDecode)
        {
            fetch->clientToDecode=new QTcpSocket();
            connect(fetch->clientToDecode,SIGNAL(connected()),this,SLOT(f2d()));
            fetch->clientToDecode->connectToHost(address,DECODE_FOR_FETCH_PORT);
            if (!fetch->clientToDecode->waitForConnected())
            {
                delete fetch->clientToDecode;
                fetch->clientToDecode=NULL;
            }else{
                connect(fetch->clientToDecode,SIGNAL(readyRead()),fetch,SLOT(dealDecodeData()));
            }
        }
    }
    if(json["ELevel"].toBool())
    {
        if(fetch && !fetch->clientToExecute)
        {
            fetch->clientToExecute=new QTcpSocket();
            connect(fetch->clientToExecute,SIGNAL(connected()),this,SLOT(f2e()));
            fetch->clientToExecute->connectToHost(address,EXECUTE_FOR_FETCH_PORT);
            if (!fetch->clientToExecute->waitForConnected())
            {
                delete fetch->clientToExecute;
                fetch->clientToExecute=NULL;
            }else{
                connect(fetch->clientToExecute,SIGNAL(readyRead()),fetch,SLOT(dealExecuteData()));
            }
        }
        if(decode && !decode->clientToExecute)
        {
            decode->clientToExecute=new QTcpSocket();
            connect(decode->clientToExecute,SIGNAL(connected()),this,SLOT(d2e()));
            decode->clientToExecute->connectToHost(address,EXECUTE_FOR_DECODE_PORT);
            if (!decode->clientToExecute->waitForConnected())
            {
                delete decode->clientToExecute;
                decode->clientToExecute=NULL;
            }else{
                connect(decode->clientToExecute,SIGNAL(readyRead()),fetch,SLOT(dealExecuteData()));
            }
        }
    }
    if(json["MLevel"].toBool())
    {
        if(fetch &&!fetch->clientToMemory)
        {
            fetch->clientToMemory=new QTcpSocket();
            connect(fetch->clientToMemory,SIGNAL(connected()),this,SLOT(f2m()));
            fetch->clientToMemory->connectToHost(address,MEMORY_FOR_FETCH_PORT);
            if (!fetch->clientToMemory->waitForConnected())
            {
                delete fetch->clientToMemory;
                fetch->clientToMemory=NULL;
            }else{
                connect(fetch->clientToMemory,SIGNAL(readyRead()),fetch,SLOT(dealMemoryData()));
            }
        }
        if(decode &&!decode->clientToMemory)
        {
            decode->clientToMemory=new QTcpSocket();
            connect(decode->clientToMemory,SIGNAL(connected()),this,SLOT(d2m()));
            decode->clientToMemory->connectToHost(address,MEMORY_FOR_DECODE_PORT);
            if (!decode->clientToMemory->waitForConnected())
            {
                delete decode->clientToMemory;
                decode->clientToMemory=NULL;
            }else{
                connect(decode->clientToMemory,SIGNAL(readyRead()),decode,SLOT(dealMemoryData()));
            }
        }
        if(execute &&!execute->clientToMemory)
        {
            execute->clientToMemory=new QTcpSocket();
            connect(execute->clientToMemory,SIGNAL(connected()),this,SLOT(e2m()));
            execute->clientToMemory->connectToHost(address,MEMORY_FOR_EXECUTE_PORT);
            if (!execute->clientToMemory->waitForConnected())
            {
                delete execute->clientToMemory;
                execute->clientToMemory=NULL;
            }
        }
    }
    if(json["WLevel"].toBool())
    {
        if(fetch && !fetch->clientToWriteback)
        {
            fetch->clientToWriteback=new QTcpSocket();
            connect(fetch->clientToWriteback,SIGNAL(connected()),this,SLOT(f2w()));
            fetch->clientToWriteback->connectToHost(address,WRITEBACK_FOR_FETCH_PORT);
            if (!fetch->clientToWriteback->waitForConnected())
            {
                delete fetch->clientToWriteback;
                fetch->clientToWriteback=NULL;
            }else{
                connect(fetch->clientToWriteback,SIGNAL(readyRead()),fetch,SLOT(dealWritebackData()));
            }
        }
        if(decode && !decode->clientToWriteback)
        {
            decode->clientToWriteback=new QTcpSocket();
            connect(decode->clientToWriteback,SIGNAL(connected()),this,SLOT(d2w()));
            decode->clientToWriteback->connectToHost(address,WRITEBACK_FOR_DECODE_PORT);
            if (!decode->clientToWriteback->waitForConnected())
            {
                delete decode->clientToWriteback;
                decode->clientToWriteback=NULL;
            }else{
                connect(decode->clientToWriteback,SIGNAL(readyRead()),decode,SLOT(dealWritebackData()));
            }
        }
        if(memory && !memory->clientToWriteback)
        {
            memory->clientToWriteback=new QTcpSocket();
            connect(memory->clientToWriteback,SIGNAL(connected()),this,SLOT(m2w()));
            memory->clientToWriteback->connectToHost(address,WRITEBACK_FOR_MEMORY_PORT);
            if (!memory->clientToWriteback->waitForConnected())
            {
                delete memory->clientToWriteback;
                memory->clientToWriteback=NULL;
            }
        }
    }
}

//master检查每个连接是否完成，完成后开始
void Y86::countConnection(int recvPool)
{
    pool=pool|recvPool;
    if(pool==0x000001ff && clockIsOk)
    {
        QJsonObject json;
        json.insert("id",3);
        broadData(json);
    }
}
//如果是主机，直接连接
//是client，发送广播连接成功
void Y86:: f2d()//3
{
    pool=pool|0x08;
}
void Y86:: f2w()//2
{
    pool=pool|0x04;
}
void Y86:: f2m()//1
{
    pool=pool|0x02;
}
void Y86:: d2e()//4
{
    pool=pool|0x10;
}
void Y86:: d2m()//5
{
    pool=pool|0x20;
}
void Y86:: d2w()//6
{
    pool=pool|0x40;
}
void Y86:: e2m()//7
{
    pool=pool|0x80;
}
void Y86:: m2w()//0
{
    pool=pool|0x01;
}
void Y86::f2e()//8
{
    pool=pool|0x100;
}

void Y86::on_PipelineRun()
{
    runState=1;
    start();
}

void Y86::on_PipelineStop()
{
    runState=0;
}

void Y86::on_PipelineStep()
{
    if(runState==0)
    {
        runState=2;
        run();
    }else{
        runState=0;
    }
}

void Y86::on_PipelineRestart()
{
    runState=4;
}

void Y86::changeCircleTime(int value)
{
    circleTime=value;
}
void broadPool(Y86* y86)
{
    QJsonObject json;
    QUdpSocket broad;
    json.insert("id",1);
    json.insert("FLevel",y86->fetch!=NULL);
    json.insert("DLevel",y86->decode!=NULL);
    json.insert("ELevel",y86->execute!=NULL);
    json.insert("MLevel",y86->memory!=NULL);
    json.insert("WLevel",y86->writeback!=NULL);
    while(!y86->stopBroadcast)
    {
        QJsonObject temp=json;
        temp.insert("pool",y86->pool);
        QByteArray bytes=QJsonDocument(temp).toBinaryData();
        broad.writeDatagram(bytes,bytes.size(),QHostAddress::Broadcast,Y86PORT);
        QThread::sleep(2);
    }
}

void Y86::newThreadBroad()
{
    QtConcurrent::run(broadPool,this);
}
void Y86::init()
{
    pool=0;
    stopBroadcast=false;
    clockIsOk=false;
    fetch=NULL;
    decode=NULL;
    execute=NULL;
    memory=NULL;
    writeback=NULL;
    clock=NULL;
    if(!listen->bind(Y86PORT,QUdpSocket::ReuseAddressHint))
    {
        QMessageBox::warning(NULL,"Warning",QString("%1端口被占用").arg(Y86PORT),QMessageBox::Ok);
    }
    connect(listen,SIGNAL(readyRead()),this,SLOT(readFromlisten()));
    connect(this,SIGNAL(on_dealData(QJsonObject,QHostAddress)),this,SLOT(dealData(QJsonObject,QHostAddress)));
}

void Y86::run()
{
    while(runState!=0)
    {
        qWarning()<<"y86"<<QThread::currentThreadId();
        if(runState==4)
        {
            clock->restartPipeline();
            runState=0;
            break;
        }
        emit nextStep();
        mutex.lock();
        awake.wait(&mutex);
        mutex.unlock();
        msleep(circleTime);
        if(runState==2)
            runState=0;
    }
}
