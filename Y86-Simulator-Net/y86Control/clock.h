#ifndef CLOCK_H
#define CLOCK_H
#include "../const.h"
#include <QObject>
class Clock:public QObject
{
    Q_OBJECT
public:
    explicit Clock();
    void ready();//开始接受连接
    void nextStep();
private slots:
    void on_newConnection();
    void on_readyRead();
signals:
    void clockIsOK();
    void stepIsDone();
private:
    int step;
    volatile int stepIsOK;//每个时钟周期检测是否所有都执行完,执行完后发射stepISDone信号
    QTcpServer *server;
    QTcpSocket *socket[5];
};

#endif // CLOCK_H
