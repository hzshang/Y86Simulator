#ifndef MEMORY_H
#define MEMORY_H
#include "const.h"
class Memory:public QThread
{
    Q_OBJECT
public:
    explicit Memory();
    ~Memory();
    void move();
    void ready();
    QTcpServer *serverForExecute;
    QTcpServer *serverForDecode;
    QTcpServer *serverForFetch;

    QTcpSocket *socketForExecute;
    QTcpSocket *socketForDecode;
    QTcpSocket *socketForFetch;
    QTcpSocket *clientToWriteback;
    QTcpSocket *clientToClock;
    void run();
private slots:
    void dealExecuteConnection();
    void dealDecodeConnection();
    void dealFetchConnection();
    void dealExecuteData();
    void circleBegin();

signals:
    void sendFromMemory(QJsonObject);
private:
    int M_stat=-1,M_icode,M_Cnd,M_valE,M_valA,M_dstE,M_dstM;
    int m_stat,m_icode,m_valE,m_valM,m_dstE,m_dstM;
    QString instruction;
    QMap<int,int> mem;
    void init();
    QJsonObject dataToWriteback();
    void sendToWriteback(QJsonObject json);
    QJsonObject dataToFetch();
    void sendToFetch(QJsonObject json);
    QJsonObject dataToDecode();
    void sendToDecode(QJsonObject json);
    void memory();
};

#endif // MEMORY_H
