#ifndef DECODE_H
#define DECODE_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Decode:QObject
{
    Q_OBJECT
public:
    explicit Decode();

    ~Decode();
    QTcpServer *serverForFetch;
    QTcpSocket *socketForFetch;
    QTcpSocket *clientToExecute;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToWriteback;
private slots:
    void dealFetchConnection();
    void dealFetchData();
    void dealExecuteData();
    void dealMemoryData();
    void dealWritebackData();
private:
    void init();
};

#endif // DECODE_H
