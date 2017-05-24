#ifndef WRITEBACK_H
#define WRITEBACK_H
#include "const.h"
class Writeback:public QObject
{
    Q_OBJECT
public:
    explicit Writeback();
    ~Writeback();
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
    void dealFetchData();
    void dealDecodeData();
    void dealMemoryData();
private:
    void init();
    void sendToDecode(QJsonObject json);
    void sendToFetch(QJsonObject json);
};

#endif // WRITEBACK_H
