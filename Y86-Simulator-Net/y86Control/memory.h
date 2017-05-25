#ifndef MEMORY_H
#define MEMORY_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
#include <QMap>
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
    int M_stat,M_icode,M_Cnd,M_valE,M_valA,M_dstE,M_dstM;
    int m_stat,m_icode,m_valE,m_valM,m_dstE,m_dstM;
    QMap<int,int> mem;
    void init();
    void memory();

};

#endif // MEMORY_H
