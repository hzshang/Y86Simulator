#ifndef EXECUTE_H
#define EXECUTE_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Execute:QObject
{
    Q_OBJECT
public:
    explicit Execute();

    ~Execute();
    QTcpServer *serverForDecode;
    QTcpSocket *socketForDecode;
    QTcpSocket *clientToMemory;
private slots:
    void dealDecodeConnection();
    void dealDecodeData();
    void dealMemoryData();
private:
    void init();

};

#endif // EXECUTE_H
