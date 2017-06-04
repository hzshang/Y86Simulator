#include "y86.h"
#include <QMetaType>


void broadPool(Y86* y86)
{
    QJsonObject json;
    json.insert("id",1);
    json.insert("FLevel",y86->fetch!=NULL);
    json.insert("DLevel",y86->decode!=NULL);
    json.insert("ELevel",y86->execute!=NULL);
    json.insert("MLevel",y86->memory!=NULL);
    json.insert("WLevel",y86->writeback!=NULL);
    QUdpSocket broad(QThread::currentThread());
    qWarning()<<"broadPool"<<QThread::currentThread();
    while(!y86->stopBroadcast)
    {
        QJsonObject temp=json;
        temp.insert("pool",y86->pool);
        QByteArray bytes=QJsonDocument(temp).toBinaryData();
        broad.writeDatagram(bytes,bytes.size(),QHostAddress::Broadcast,Y86PORT);
        QThread::sleep(2);
    }
}


Y86::Y86()
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    listen=new QUdpSocket(this);
    broadcast=new QUdpSocket(this);
    circleTime=50;
    master=false;
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
    runState=0;
    clock=NULL;
    if(!listen->bind(Y86PORT,QUdpSocket::ReuseAddressHint))
    {
        QMessageBox::warning(NULL,"Warning",QString("%1端口被占用").arg(Y86PORT),QMessageBox::Ok);
    }
    qWarning()<<"init"<<QThread::currentThread();
    connect(listen,SIGNAL(readyRead()),this,SLOT(readFromlisten()));
    connect(this,SIGNAL(on_dealData(QJsonObject,QHostAddress)),this,SLOT(dealData(QJsonObject,QHostAddress)));
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
    QtConcurrent::run(broadPool,this);
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
