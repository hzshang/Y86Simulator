#ifndef EXECUTE_H
#define EXECUTE_H
#include "const.h"
class Execute:public QObject
{
    Q_OBJECT
public:
    explicit Execute();

    ~Execute();
    QTcpServer *serverForDecode;
    QTcpServer *serverForFetch;
    QTcpSocket *socketForDecode;
    QTcpSocket *socketForFetch;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToClock;
private slots:
    void dealDecodeConnection();
    void dealFetchConnection();
//    void dealFetchData();
    void dealDecodeData();
//    void dealMemoryData();
private:
    void init();
    void sendToMemory(QJsonObject json);
    void sendToFetch(QJsonObject json);
};

#endif // EXECUTE_H
