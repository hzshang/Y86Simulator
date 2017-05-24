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
    QTcpSocket *socketForDecode;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToClock;
private slots:
    void dealDecodeConnection();
    void dealDecodeData();
    void dealMemoryData();
private:
    void init();
    void sendToMemory(QJsonObject json);
};

#endif // EXECUTE_H
