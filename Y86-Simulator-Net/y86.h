#ifndef Y86_H
#define Y86_H
#include "y86Control/fetch.h"
#include "y86Control/decode.h"
#include "y86Control/execute.h"
#include "y86Control/memory.h"
#include "y86Control/writeback.h"
#include "y86Control/clock.h"
#include "const.h"
class Y86: public QThread
{
    Q_OBJECT
    Q_ENUMS(state)
public:
    explicit Y86(QObject *parent=0);
    void broadData(QJsonObject &data);
    float sleepTime;
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
    void on_clockIsOK();
    void on_stepIsDone();
    void begin();//开始流水线,仅对master有用
    void pause();//暂停流水线，仅对master有用
    void initPipeline();//初始化流水线
    void f2d();
    void f2w();
    void f2m();
    void d2e();
    void d2m();
    void d2w();
    void e2m();
    void m2w();
    void f2e();

    void on_PipelineRun();
    void on_PipelineStop();
    void on_PipelineStep();
    void on_PipelineRestart();
private:
    Fetch *fetch;
    Decode *decode;
    Execute *execute;
    Memory *memory;
    Writeback *writeback;
    QUdpSocket *listen;
    QUdpSocket *broadcast;
    Clock *clock;
    int circleTime;//周期时间，单位ms
    volatile int runState;//运行状态  running 1,step 2,pause 0 ,4 restart 仅对master有效
    volatile bool stopBroadcast;
    volatile bool stepIsDone;
    volatile int pool;
    bool clockIsOk;//连接阶段，如果五个阶段都连上了clock，则设为true  仅对master有效
    bool master;//如果含有F，则设为true
    void init();
    void beginPipeLine();
    void beignConnect(QJsonObject,QHostAddress);
    void countConnection(int recvPool);
};

#endif // Y86_H
