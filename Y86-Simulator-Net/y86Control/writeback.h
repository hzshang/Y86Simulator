#ifndef WRITEBACK_H
#define WRITEBACK_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Writeback:QObject
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
private slots:
    void dealFetchConnection();
    void dealDecodeConnection();
    void dealMemoryConnection();
    void dealFetchData();
    void dealDecodeData();
    void dealMemoryData();
private:
    void init();
};

#endif // WRITEBACK_H
