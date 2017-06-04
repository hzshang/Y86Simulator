#ifndef FETCH_H
#define FETCH_H
#include "const.h"
class Fetch:public QThread
{
    Q_OBJECT
public:
    explicit Fetch();

    QTcpSocket *clientToDecode;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToWriteback;
    QTcpSocket *clientToClock;
    QTcpSocket *clientToExecute;
    void run();
    void move();
private slots:
    void receiveInstr(QString);
    void dealExecuteData();
    void dealMemoryData();
    void dealWritebackData();
    void dealDecodeData();
    void circleBegin();
signals:
    void sendFromFetch(QJsonObject);
private:
    int predPC = 0;
    int PC = 0;
    bool isRet = false;
    bool isRisk = false;
    bool isEnd = false;
    int f_stat = -1,f_icode = -1,f_ifun,f_rA,f_rB,f_valC,f_valP;

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
    void circleBegin2();
    bool clockmark;
    QMutex mutexNum;
    int doneNum;
};

#endif // FETCH_H
