#ifndef EXECUTE_H
#define EXECUTE_H
#include "const.h"
class Execute:public QThread
{
    Q_OBJECT
public:
    explicit Execute();
    void run();
    ~Execute();
    void move();
    QTcpServer *serverForDecode;
    QTcpServer *serverForFetch;
    QTcpSocket *socketForDecode;
    QTcpSocket *socketForFetch;
    QTcpSocket *clientToMemory;
    QTcpSocket *clientToClock;
private slots:
    void dealDecodeConnection();
    void dealFetchConnection();
    void dealDecodeData();
    void circleBegin();

signals:
    void sendFromExecute(QJsonObject);
    void sendCC(QJsonObject);
private:
    int E_stat=-1,E_icode,E_ifun,E_valC,E_valA,E_valB,E_dstE,E_dstM;
    int E_srcA,E_srcB;
    int aluA,aluB;
    int ZF = 0,SF = 0,OF = 0;
    int e_stat,e_icode,e_Cnd,e_valE,e_valA,e_dstE,e_dstM;
    QString instruction;
    bool isEnd  = false;

    void init();
    QJsonObject dataToMemory();
    void sendToMemory(QJsonObject json);
    QJsonObject dataToFetch();
    void sendToFetch(QJsonObject json);
    QJsonObject dataToDecode();
    void sendToDecode(QJsonObject json);
    QJsonObject CCData();
    void ALU_A();
    void ALU_B();
    void execute();
    void setCC();
};

#endif // EXECUTE_H
