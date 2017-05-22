#ifndef Y86_H
#define Y86_H
#include "y86Control/fetch.h"
#include "y86Control/decode.h"
#include "y86Control/execute.h"
#include "y86Control/memory.h"
#include "y86Control/writeback.h"
#include "const.h"
#include <QObject>
#include <QMessageBox>
#include <QString>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>
class Y86: public QThread
{
    Q_OBJECT
public:
    explicit Y86(QObject *parent=0);
    void broadData(QJsonObject &data);
protected:
    void run();
signals:
    void on_dealData(QJsonObject,QHostAddress);
    void cancelConnectionSginal();
    void showPipeLine(Y86 *);
private slots:
    void ready(bool FLevel,bool DLevel,bool ELevel,bool MLevel,bool WLevel);
    void readFromlisten();
    void dealData(QJsonObject,QHostAddress);
    void f2d();
    void f2w();
    void f2m();
    void d2e();
    void d2m();
    void d2w();
    void e2m();
    void m2w();
private:
    Fetch *fetch;
    Decode *decode;
    Execute *execute;
    Memory *memory;
    Writeback *writeback;
    QUdpSocket *listen;
    QUdpSocket *broadcast;
    volatile bool stop;
    volatile int pool;
    bool master;
    void init();
    void beginPipeLine();
    void beignConnect(QJsonObject,QHostAddress);
    void countConnection(int recvPool);
};

#endif // Y86_H
