#ifndef EXECUTE_H
#define EXECUTE_H
#include <QUdpSocket>
#include <QTcpSocket>
#include <QTcpServer>
#include <QObject>
class Execute:QObject
{
    Q_OBJECT
public:
    explicit Execute();

    ~Execute();
    QTcpServer *serverForDecode;
    QTcpSocket *socketForDecode;
    QTcpSocket *clientToMemory;
private slots:
    void dealDecodeConnection();
    void dealDecodeData();
    void dealMemoryData();
private:
    int E_stat,E_icode,E_ifun,E_valC,E_valA,E_valB,E_dstE,E_dstM;
    int aluA,aluB;
    int ZF,SF,OF;
    int e_stat,e_icode,e_Cnd,e_valE,e_valA,e_dstE,e_dstM;

    void init();
    void ALU_A();
    void ALU_B();
    void execute();
    void setCC();

};

#endif // EXECUTE_H
