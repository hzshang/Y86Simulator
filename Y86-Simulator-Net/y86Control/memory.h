#ifndef MEMORY_H
#define MEMORY_H
#include "const.h"
class Memory:public QObject
{
    Q_OBJECT
public:
    explicit Memory();
    ~Memory();

    void ready();
    QTcpServer *serverForExecute;
    QTcpServer *serverForDecode;
    QTcpServer *serverForFetch;

    QTcpSocket *socketForExecute;
    QTcpSocket *socketForDecode;
    QTcpSocket *socketForFetch;
    QTcpSocket *clientToWriteback;
    QTcpSocket *clientToClock;
private slots:
    void dealExecuteConnection();
    void dealDecodeConnection();
    void dealFetchConnection();
    void dealExecuteData();
    void dealDecodeData();
    void dealFetchData();
    void dealWritebackData();
private:
    void init();
    void sendToWriteback(QJsonObject json);
    void sendToFetch(QJsonObject json);
    void sendToDecode(QJsonObject json);
};

#endif // MEMORY_H
