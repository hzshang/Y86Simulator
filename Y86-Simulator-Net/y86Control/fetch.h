#ifndef FETCH_H
#define FETCH_H
#include "const.h"
class Fetch:public QObject
{
    Q_OBJECT
public:
    explicit Fetch();
    ~Fetch();

    QTcpSocket *clientToDecode;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToWriteback;
    QTcpSocket *clientToClock;
    QTcpSocket *clientToExecute;
private slots:
    void receiveInstr(QString);
    void dealDecodeData();
    void dealMemoryData();
    void dealWritebackData();
    void dealExecuteData();
private:
    int predPC = 0;
    int PC = 0;
    int f_stat,f_icode,f_ifun,f_rA,f_rB,f_valC,f_valP;

    int M_icode=-1,M_valA=-1,M_Cnd=-1;
    int W_icode=-1,W_valM=-1;

    QString instruction;
    QVector<int> instrCode;
    void  select_PC();
    void fetch();
    void  predict_PC();
    int hexTodec(QString str);
    void switchStrToInt();
    int getValue(int l,int r);
    void init();
    void sendToDecode(QJsonObject json);
//    void sendToMemory(QJsonObject json);
//    void sendToWriteback(QJsonObject json);
};

#endif // FETCH_H
