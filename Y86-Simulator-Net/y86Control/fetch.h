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
    void dealExecuteData();
    void dealMemoryData();
    void dealWritebackData();
    void dealDecodeData();
    void dealClockData();
signals:
    void sendFromFetch(QJsonObject);
private:
    int predPC = 0;
    int PC = 0;
    bool isRet = false;
    bool isRisk = false;
    int f_stat,f_icode,f_ifun,f_rA,f_rB,f_valC,f_valP;

    int M_icode=-1,M_valA=-1,M_Cnd=-1;
    int W_icode=-1,W_valM=-1;
    int E_icode=-1,e_Cnd=-1,E_dstM=-1;
    int d_srcA=-1,d_srcB=-1;

    QString instruction;
    QVector<int> instrCode;
    QString instrString;

    void init();
    void  select_PC();
    QString getRegStr(int k);
    QString getInstruction();
    void fetch();
    void  predict_PC();
    int hexTodec(QString str);
    void switchStrToInt();
    int getValue(int l,int r);
    QJsonObject DataToDecode();
    void sendToDecode(QJsonObject json);
    QJsonObject dataToMainWindow();
};

#endif // FETCH_H
