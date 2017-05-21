#include "y86.h"
#include <QMetaType>
Y86::Y86(QObject *parent)
{
    qRegisterMetaType<QHostAddress>("QHostAddress");
    listen=new QUdpSocket();
    broadcast=new QUdpSocket();
    if(!listen->bind(Y86PORT,QUdpSocket::ReuseAddressHint))
    {
        QMessageBox::warning(NULL,"Warning",QString("%1端口被占用").arg(Y86PORT),QMessageBox::Ok);
    }
    connect(listen,SIGNAL(readyRead()),this,SLOT(readFromlisten()));
    connect(this,SIGNAL(on_dealData(QJsonObject,QHostAddress)),this,SLOT(dealData(QJsonObject,QHostAddress)),Qt::QueuedConnection);

}
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
//按下取消按钮时调用
void Y86::cancel()
{
    QJsonObject json;
    json.insert("id",0);
    broadData(json);
    emit cancelConnectionSginal();
}

void Y86::readFromlisten()
{
    while(listen->hasPendingDatagrams())
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
    case 0:
        cancelConnect();
        break;
    case 1:
        beignConnect(json,address);
        break;
    case 2:
        if(master)
            countConnection(json["cid"].toInt());
        break;
    case 3:
        beginPipeLine();
        break;
    }
}
//取消连接
void Y86::cancelConnect()
{
    delete fetch;
    delete decode;
    delete execute;
    delete memory;
    delete writeback;
}
void Y86::beginPipeLine()
{
    qWarning()<<"Begin";
    stop=true;
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
//检查每个连接是否完成
//完成后广播开始
void Y86::countConnection(int cid)
{
    pool[cid]=true;
    qWarning()<<"Connected:"<<cid;
    int i;
    for(i=0;i<8&&pool[i];)
        i++;
    if(i==8)
    {
        QJsonObject json;
        json.insert("id",3);
        broadData(json);
    }
}
//如果是主机，直接加
//是client，发送广播连接成功
void Y86:: f2d()
{
    if(master)
        countConnection(3);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",3);
        broadData(json);
    }
}
void Y86:: f2w()
{
    if(master)
        countConnection(1);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",1);
        broadData(json);
    }
}
void Y86:: f2m()
{

    if(master)
        countConnection(2);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",2);
        broadData(json);
    }
}
void Y86:: d2e()
{
    if(master)
        countConnection(4);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",4);
        broadData(json);
    }
}
void Y86:: d2m()
{
    if(master)
        countConnection(5);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",5);
        broadData(json);
    }
}
void Y86:: d2w()
{
    if(master)
        countConnection(6);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",6);
        broadData(json);
    }
}
void Y86:: e2m()
{

    if(master)
        countConnection(7);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",7);
        broadData(json);
    }
}
void Y86:: m2w()
{

    if(master)
        countConnection(0);
    else
    {
        QJsonObject json;
        json.insert("id",2);
        json.insert("cid",0);
        broadData(json);
    }
}
void Y86::init()
{
    for(int i=0;i<8;i++)
        pool[i]=false;
    stop=false;
    fetch=NULL;
    decode=NULL;
    execute=NULL;
    memory=NULL;
    writeback=NULL;
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
        broadData(json);
        sleep(1);
    }
    quit();
}
