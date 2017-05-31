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
public:
    explicit Y86();
    void broadData(QJsonObject &data);
    float sleepTime;
    bool master;//如果含有F，则设为true
protected:
    void run();

signals:
    void on_dealData(QJsonObject,QHostAddress);
    void cancelConnectionSginal();
    void showPipeLine(Y86 *);
    void nextStep();
private slots:
    void ready(bool FLevel,bool DLevel,bool ELevel,bool MLevel,bool WLevel);
    void readFromlisten();
    void dealData(QJsonObject,QHostAddress);
    void on_clockIsOK();
    void begin();//开始流水线,仅对master有用
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
    void changeCircleTime(int);
private:
    Fetch *fetch;
    Decode *decode;
    Execute *execute;
    Memory *memory;
    Writeback *writeback;
    QUdpSocket *listen;
    QUdpSocket *broadcast;
    Clock *clock;
    QMutex mutex;
    QWaitCondition awake;
    int circleTime;//周期时间，单位ms
    volatile int runState;//运行状态  running 1,step 2,pause 0 ,4 restart 仅对master有效
    volatile bool stopBroadcast;
    volatile int pool;
    bool clockIsOk;//连接阶段，如果五个阶段都连上了clock，则设为true  仅对master有效

    void init();
    void beginPipeLine();
    void beignConnect(QJsonObject,QHostAddress);
    void countConnection(int recvPool);
};

#endif // Y86_H
