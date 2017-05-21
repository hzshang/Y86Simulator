#ifndef MEMORY_H
#define MEMORY_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Memory:QObject
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
};

#endif // MEMORY_H
