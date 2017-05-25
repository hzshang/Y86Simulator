#include "execute.h"
#include "const.h"
Execute::Execute()
{
    init();
}

void Execute::init()
{
    socketForDecode=NULL;
    clientToMemory=NULL;
    serverForDecode=new QTcpServer();
    serverForDecode->listen(QHostAddress::Any,EXECUTE_FOR_DECODE_PORT);
    connect(serverForDecode,SIGNAL(newConnection()),this,SLOT(dealDecodeConnection()));
}

Execute::~Execute()
{
    delete serverForDecode;
}
void Execute::dealDecodeConnection()
{
    socketForDecode=serverForDecode->nextPendingConnection();
    connect(socketForDecode,SIGNAL(readyRead()),this,SLOT(dealDecodeData()));
    serverForDecode->pauseAccepting();
}

void Execute::dealDecodeData()
{
    //get:E_stat,E_icode,E_ifun,E_valC,E_valA,E_valB,E_dstE,E_dstM;
}

void Execute::dealMemoryData()
{

}

//预处理aluA的值
void Execute::ALU_A()
{
    switch (E_icode) {
    case 1:
        break;
    case 2:
        aluA = E_valA;
        break;
    case 3:
        aluA = E_valC;
        break;
    case 4:
        aluA = E_valC;
        break;
    case 5:
        aluA = E_valC;
        break;
    case 6:
        aluA = E_valA;
        break;
    case 8:
        aluA = -4;
        break;
    case 9:
        aluA = 4;
        break;
    case 10:
        aluA  = -4;
        break;
    case 11:
        aluA = 4;
        break;
    default:
        break;
    }
}

//预处理aluB的值
void Execute::ALU_B()
{
    switch (E_icode) {
    case 1:
        break;
    case 2:
    case 3:
        aluB = 0;
        break;
    case 4:
    case 5:
    case 6:
    case 8:
    case 9:
    case 10:
    case 11:
        aluB = E_valB;
        break;
    default:
        break;
    }
}

void Execute::setCC()
{
    if(e_valE == 0)
        ZF = 1;
    else ZF = 0;
    if(e_valE < 0)
        SF = 1;
    else SF = 0;
    if((aluA < 0 == aluB < 0) && (e_valE < 0 != aluA < 0))
        OF = 1;
    else OF = 0;
}

void Execute::execute()
{
    e_stat = E_stat;
    if(e_icode == 0)
        return;
    e_icode = E_icode;
    e_dstM = E_dstM;
    ALU_A();
    ALU_B();
    switch (E_icode) {
    case 1:
        break;
    case 2:
    case 3:
    case 4:
    case 5:
    case 8:
    case 9:
    case 10:
    case 11:
        e_valE = aluB + aluA;
        break;
    case 6:
        switch (E_ifun) {
        case 0:
            e_valE = aluB + aluA;
            break;
        case 1:
            e_valE = aluB - aluA;
            break;
        case 2:
            e_valE = aluB & aluA;
            break;
        case 3:
            e_valE = aluB ^ aluA;
            break;
        default:
            break;
        }
        setCC();
        break;
    case 7:
        switch (E_icode) {
        case 0:
            e_Cnd = 1;
            break;
        case 1:
            if(SF || ZF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 2:
            if(SF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 3:
            if(ZF)
                e_Cnd =1;
            else e_Cnd = 0;
            break;
        case 4:
            if(!ZF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 5:
            if(!SF)
                e_Cnd = 1;
            else e_Cnd = 0;
            break;
        case 6:
            if(!SF && !ZF)
                e_Cnd = 1;
            else e_Cnd =0;
            break;
        default:
            break;
        }
        break;
    default:
        break;
    }
}
