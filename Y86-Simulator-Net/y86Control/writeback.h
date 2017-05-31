#ifndef WRITEBACK_H
#define WRITEBACK_H
#include "const.h"
class Writeback:public QThread
{
    Q_OBJECT
public:
    explicit Writeback();
    ~Writeback();
    void move();
    QTcpServer *serverForFetch;
    QTcpSocket *socketForFetch;

    QTcpServer *serverForDecode;
    QTcpSocket *socketForDecode;

    QTcpServer *serverForMemory;
    QTcpSocket *socketForMemory;
    QTcpSocket *clientToClock;
    void run();//在run里执行每个时钟周期的任务
private slots:
    void dealFetchConnection();
    void dealDecodeConnection();
    void dealMemoryConnection();
    void dealMemoryData();
    void circleBegin();
signals:
    void sendFromWriteback(QJsonObject);
private:
    int W_stat=-1,W_icode,W_valE,W_valM,W_dstE,W_dstM;
    QString instruction;
    void writeReg(int dst,int val);
    void writeback();
    void init();
    QJsonObject dataToDecode();
    void sendToDecode(QJsonObject json);
    QJsonObject dataToFetch();
    void sendToFetch(QJsonObject json);
};

#endif // WRITEBACK_H
