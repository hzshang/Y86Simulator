#ifndef EXECUTE_H
#define EXECUTE_H
#include "const.h"
class Execute:public QObject
{
    Q_OBJECT
public:
    explicit Execute();

    ~Execute();
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
    void dealClockData();

signals:
    void sendFromExecute(QJsonObject);
    void sendCC(int,int,int);
private:
    int E_stat=-1,E_icode,E_ifun,E_valC,E_valA,E_valB,E_dstE,E_dstM;
    int E_srcA,E_srcB;
    int aluA,aluB;
    int ZF,SF,OF;
    int e_stat,e_icode,e_Cnd,e_valE,e_valA,e_dstE,e_dstM;
    QString instruction;

    void init();
    QJsonObject dataToMemory();
    void sendToMemory(QJsonObject json);
    QJsonObject dataToFetch();
    void sendToFetch(QJsonObject json);
    QJsonObject dataToDecode();
    void sendToDecode(QJsonObject json);
    void ALU_A();
    void ALU_B();
    void execute();
    void setCC();
};

#endif // EXECUTE_H
