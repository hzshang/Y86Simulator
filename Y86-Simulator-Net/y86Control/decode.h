#ifndef DECODE_H
#define DECODE_H
#include "const.h"
class Decode:public QObject
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
    QTcpSocket *clientToClock;
private slots:
    void dealFetchConnection();
    void dealFetchData();
//    void dealExecuteData();
    void dealMemoryData();
    void dealWritebackData();
    void dealClockData();

signals:
    void sendFromDecode(QJsonObject);
private:
    void init();

    int D_stat=-1,D_icode,D_ifun,D_rA,D_rB,D_valC,D_valP;
    int d_stat,d_icode,d_ifun,d_valC,d_valA,d_valB,d_dstE,d_dstM,d_srcA,d_srcB;
    int icodeStorage;
    QString instruction;
    bool isRisk = false;

    int E_icode=-1,e_Cnd=-1,E_dstM=-1,e_dstE=-1,e_valE=-1;
    int M_dstM=-1,m_valM=-1,M_dstE=-1,M_valE=-1;
    int W_dstM=-1,W_valM=-1,W_dstE=-1,W_valE=-1;

    int getRegValue(int src);
    void decode();
    void sel_fwd_valA();
    void fwd_valB();

    QJsonObject dataToExecute();
    void sendToExecute(QJsonObject json);
    QJsonObject dataToFetch();
    void sendToFetch(QJsonObject json);

//    void sendToMemory(QJsonObject json);
//    void sendToWriteback(QJsonObject json);
};

#endif // DECODE_H
