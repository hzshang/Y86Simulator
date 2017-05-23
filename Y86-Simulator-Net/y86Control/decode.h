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
    int D_stat,D_icode,D_ifun,D_rA,D_rB,D_valC,D_valP;
    int d_stat,d_icode,d_ifun,d_valC,d_valA,d_valB,d_dstE,d_dstM,d_srcA,d_srcB;

    int getRegValue(int src);
    void decode();
    void sel_fwd_valA();
    void fwd_valB();

};

#endif // DECODE_H
