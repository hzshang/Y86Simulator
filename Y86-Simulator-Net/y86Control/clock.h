#ifndef CLOCK_H
#define CLOCK_H
#include "../const.h"
#include <QObject>
class Clock:public QThread
{
    Q_OBJECT
public:
    explicit Clock(QWaitCondition*);
    void ready();//开始接受连接
    void restartPipeline();
    void run();
    void move();
private slots:
    void on_FetchConnection();
    void on_DecodeConnection();
    void on_ExecuteConnection();
    void on_MemoryConnection();
    void on_WritebackConnection();
    void on_readyRead();
    void nextStep();
signals:
    void clockIsOK();
private:
    int step;
    volatile int stepIsOK;//每个时钟周期检测是否所有都执行完,执行完后发射stepISDone信号
    QTcpServer *server[5];
    QTcpSocket *socket[5];
    QWaitCondition *y86Awake;
    QMutex mutex;
};

#endif // CLOCK_H
