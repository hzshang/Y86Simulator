#include "y86.h"
#include <QMetaType>
Y86::Y86(QObject *parent)
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    listen=new QUdpSocket();
    broadcast=new QUdpSocket();
}
//发射广播寻找局域网客户端，建立连接
void Y86::ready(bool FLevel, bool DLevel, bool ELevel, bool MLevel, bool WLevel)
{
    init();
    if(FLevel)
    {
        fetch=new Fetch();
        master=true;
    }
    if(DLevel)
        decode=new Decode();
    if(ELevel)
        execute=new Execute();
    if(MLevel)
        memory=new Memory();
    if(WLevel)
        writeback=new Writeback();
    start();
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
    qWarning()<<"send"<<data;
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

void Y86::beginPipeLine()
{
    qWarning()<<"Begin";
    stop=true;
    emit showPipeLine(this);
}

void Y86::beignConnect(QJsonObject json,QHostAddress address)
{
    if(json["DLevel"].toBool())
    {
        if(fetch && !fetch->clientToDecode)
        {
            fetch->clientToDecode=new QTcpSocket();
            connect(fetch->clientToDecode,SIGNAL(connected()),this,SLOT(f2d()));
            fetch->clientToDecode->connectToHost(address,DECODE_FOR_FETCH_PORT);
        }
    }
    if(json["ELevel"].toBool())
    {
        if(decode && !decode->clientToExecute)
        {
            decode->clientToExecute=new QTcpSocket();
            connect(decode->clientToExecute,SIGNAL(connected()),this,SLOT(d2e()));
            decode->clientToExecute->connectToHost(address,EXECUTE_FOR_DECODE_PORT);
        }
    }
    if(json["MLevel"].toBool())
    {
        if(fetch &&!fetch->clientToMemory)
        {
            fetch->clientToMemory=new QTcpSocket();
            connect(fetch->clientToMemory,SIGNAL(connected()),this,SLOT(f2m()));
            fetch->clientToMemory->connectToHost(address,MEMORY_FOR_FETCH_PORT);
        }
        if(decode &&!decode->clientToMemory)
        {
            decode->clientToMemory=new QTcpSocket();
            connect(decode->clientToMemory,SIGNAL(connected()),this,SLOT(d2m()));
            decode->clientToMemory->connectToHost(address,MEMORY_FOR_DECODE_PORT);
        }
        if(execute &&!execute->clientToMemory)
        {
            execute->clientToMemory=new QTcpSocket();
            connect(execute->clientToMemory,SIGNAL(connected()),this,SLOT(e2m()));
            execute->clientToMemory->connectToHost(address,MEMORY_FOR_EXECUTE_PORT);
        }
    }
    if(json["WLevel"].toBool())
    {
        if(fetch && !fetch->clientToWriteback)
        {
            fetch->clientToWriteback=new QTcpSocket();
            connect(fetch->clientToWriteback,SIGNAL(connected()),this,SLOT(f2w()));
            fetch->clientToWriteback->connectToHost(address,WRITEBACK_FOR_FETCH_PORT);
        }
        if(decode && !decode->clientToWriteback)
        {
            decode->clientToWriteback=new QTcpSocket();
            connect(decode->clientToWriteback,SIGNAL(connected()),this,SLOT(d2w()));
            decode->clientToWriteback->connectToHost(address,WRITEBACK_FOR_DECODE_PORT);
        }
        if(memory && !memory->clientToWriteback)
        {
            memory->clientToWriteback=new QTcpSocket();
            connect(memory->clientToWriteback,SIGNAL(connected()),this,SLOT(m2w()));
            memory->clientToWriteback->connectToHost(address,WRITEBACK_FOR_MEMORY_PORT);
        }
    }
}

//master检查每个连接是否完成，完成后开始
void Y86::countConnection(int recvPool)
{
    pool=pool|recvPool;
    if(pool==0x000000ff)
    pool=pool|recvPool;
    if(pool==0x000000ff)

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
void Y86::init()
{
    pool=0;
    stop=false;
    fetch=NULL;
    decode=NULL;
    execute=NULL;
    memory=NULL;
    writeback=NULL;
    if(!listen->bind(Y86PORT,QUdpSocket::ReuseAddressHint))
    {
        QMessageBox::warning(NULL,"Warning",QString("%1端口被占用").arg(Y86PORT),QMessageBox::Ok);
    }
    connect(listen,SIGNAL(readyRead()),this,SLOT(readFromlisten()));
    connect(this,SIGNAL(on_dealData(QJsonObject,QHostAddress)),this,SLOT(dealData(QJsonObject,QHostAddress)));
}

void Y86::run()
{
    QJsonObject json;
    json.insert("id",1);
    json.insert("FLevel",fetch!=NULL);
    json.insert("DLevel",decode!=NULL);
    json.insert("ELevel",execute!=NULL);
    json.insert("MLevel",memory!=NULL);
    json.insert("WLevel",writeback!=NULL);
    while(!stop)
    {
        QJsonObject temp=json;
        temp.insert("pool",pool);
        broadData(temp);
        sleep(2);
    }
    qWarning()<<"NO BROAD!";
    disconnect(listen,SIGNAL(readyRead()),this,SLOT(readFromlisten()));
    quit();
}
