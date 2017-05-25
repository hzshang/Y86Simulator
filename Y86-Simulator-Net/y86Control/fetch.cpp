#include "fetch.h"
#include "const.h"
#include "globle.h"
#include "mainwindow.h"
extern int broadcast;
Fetch::Fetch()
{
    init();
}

Fetch::~Fetch()
{

}

void Fetch::init()
{
    clientToDecode=NULL;
    clientToMemory=NULL;
    clientToWriteback=NULL;
    clientToClock=NULL;
    clientToExecute=NULL;
}
void Fetch::sendToDecode(QJsonObject json)
{
    if(clientToDecode->state()==QAbstractSocket::UnconnectedState)
    {
        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
        return;
    }
    QByteArray bytes=QJsonDocument(json).toBinaryData();
    clientToDecode->write(bytes);
}

void Fetch::dealDecodeData()
{
    //get:D_icode;
}

void Fetch::dealExecuteData()
{
    //get:E_icode,e_Cnd;
}
/*
//void Fetch::sendToMemory(QJsonObject json)
//{
//    if(clientToMemory->state()==QAbstractSocket::UnconnectedState)
//    {
//        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
//        return;
//    }
//    QByteArray bytes=QJsonDocument(json).toBinaryData();
//    clientToMemory->write(bytes);
//}

//void Fetch::sendToWriteback(QJsonObject json)
//{
//    if(clientToWriteback->state()==QAbstractSocket::UnconnectedState)
//    {
//        QMessageBox::warning(NULL,"Warning",QString("已断开连接"),QMessageBox::Ok);
//        return;
//    }
//    QByteArray bytes=QJsonDocument(json).toBinaryData();
//    clientToWriteback->write(bytes);
//}

//void Fetch::dealDecodeData()
//{
//    //不用写该函数
//}
*/
void Fetch::dealMemoryData()
{
    //get M_icode&M_valA&M_Cnd;
}

void Fetch::dealWritebackData()
{
    //get W_icode&W_valM;
}

void Fetch::dealExecuteData()
{

}

void Fetch::receiveInstr(QString str)
{
    instruction = str;
}

//将字符串形式的十六进制数据转为十进制,在switchStrToInt()函数中调用
int Fetch::hexTodec(QString str)
{
    if(str == "a")
        return 10;
    else if(str == "b")
        return 11;
    else if(str == "c")
        return 12;
    else if(str == "d")
        return 13;
    else if(str == "e")
        return 14;
    else if(str == "f")
        return 15;
    else
    {
        int tmp = str.toInt();
        switch (tmp) {
        case 0:
            return 0;
        case 1:
            return 1;
        case 2:
            return 3;
        case 3:
            return 4;
        case 5:
            return 5;
        case 6:
            return 6;
        case 7:
            return 7;
        case 8:
            return 8;
        case 9:
            return 9;
        default:
            return -1;
        }
    }
}

//指令改为十进制形式，存储在数组中
void Fetch::switchStrToInt()
{
    int i = 0;
    int length = instruction.length();
    for(i = 0; i < length; i++)
    {
        instrCode.append(hexTodec(instruction.mid(i,1)));
    }
}

//获取valP或者valC的值
int Fetch::getValue(int l, int r)
{
    int i;
    int value = 0;
    for(i = r;i > l;)
    {
        value = 16*value +instrCode[i-1];
        value = 16*value+instrCode[i];
        i = i-2;
    }
    return value;
}

void Fetch::select_PC()
{
    if(M_icode == 7 && !M_Cnd)
        PC = M_valA;
    else if(W_icode == 9)
    {
        PC = W_valM;
        isRet = false;
    }
    else if(isRisk)
    {
        PC = PC;
        isRisk = false;
    }
    else
        PC = predPC;
}

void Fetch::fetch()
{   
    //分支错误处理
    if(E_icode == 7 && !e_Cnd)
        f_icode = 1;
    //正常取值
    else{
        f_icode = instrCode[PC];
        f_ifun = instrCode[PC+1];
    }

    if(f_icode == globle::HTL)
        return;
    switch (f_icode) {
    case 0:
        f_stat = 1;
        break;
    case 1:
        if(!isRet)//ret处理
        f_valP = PC + 2;
        else f_valP = PC;
        f_stat = 0;
        break;
    case 2:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valP = PC + 4;
        f_stat = 0;
        break;
    case 3:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valC = getValue(PC+4,PC+11);
        f_valP = PC + 12;
        f_stat = 0;
        break;
    case 4:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valC = getValue(PC+4,PC+11);
        f_valP = PC + 12;
        f_stat = 0;
        break;
    case 5:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valC = getValue(PC+4,PC+11);
        f_valP = PC + 12;
        f_stat = 0;
        break;
    case 6:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valP = PC + 4;
        f_stat = 0;
        break;
    case 7:
        f_valC = getValue(PC+2,PC+9);
        f_valP = PC + 10;
        f_stat = 0;
        break;
    case 8:
        f_valC = getValue(PC+2,PC+9);
        f_valP = PC + 10;
        f_stat = 0;
        break;
    case 9:
        f_valP = PC +2;
        isRet = true;
        f_stat = 0;
        break;
    case 10:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valP = PC + 4;
        f_stat = 0;
        break;
    case 11:
        f_rA = instrCode[PC+2];
        f_rB = instrCode[PC+3];
        f_valP = PC + 4;
        f_stat = 0;
        break;
    default:
        f_stat = 3;//无效的指令
        break;
    }
    if((E_icode == 5 || E_icode == 11) &&
            (E_dstM == d_srcA || E_dstM == d_srcB))//加载使用数据冒险，暂停效果
        isRisk = true;

}

void Fetch::predict_PC()
{
    if(f_icode == 7 || f_icode == 8)
        predPC = f_valC;
    else
        predPC = f_valP;
}

void Fetch::sendFromFetch(QMap<QString,int> send)
{
    if(f_stat != 0)
        send["stat"] = f_stat;
    else
    {
        send["stat"] = f_stat;
        send["PC"] = PC;
        send["predPC"] = predPC;
    }

}
