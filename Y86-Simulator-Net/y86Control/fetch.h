#ifndef FETCH_H
#define FETCH_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Fetch:QObject
{
    Q_OBJECT
public:
    explicit Fetch();
    ~Fetch();

    QTcpSocket *clientToDecode;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToWriteback;
private slots:
    void dealDecodeData();
    void dealMemoryData();
    void dealWritebackData();
private:
    int predPC;
    void init();
};

#endif // FETCH_H
