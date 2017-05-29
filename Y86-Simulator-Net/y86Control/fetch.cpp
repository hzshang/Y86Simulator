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

//生成发送到Decode阶段的数据
QJsonObject Fetch::DataToDecode()
{
    QJsonObject sendData;
    if(f_stat != 0)
    {
        sendData.insert("D_stat",f_stat);
        return sendData;
    }
    sendData.insert("D_stat",f_stat);
    sendData.insert("D_icode",f_icode);
    sendData.insert("D_ifun",f_ifun);
    sendData.insert("D_valP",f_valP);
    sendData.insert("instruction",instrString);
    switch (f_icode) {
    case 1:
    case 9:
        return sendData;
    case 2:
    case 6:
    case 10:
    case 11:
        sendData.insert("D_rA",f_rA);
        sendData.insert("D_rB",f_rB);
        return sendData;
    case 3:
    case 4:
    case 5:
    case 7:
    case 8:
        sendData.insert("D_rA",f_rA);
        sendData.insert("D_rB",f_rB);
        sendData.insert("D_valC",f_valC);
        return sendData;
    }
    return sendData;
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

void Fetch:: dealMemoryData()
{
    //get M_icode&M_valA&M_Cnd;
    QByteArray bytes=clientToMemory->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("M_icode"))
    {
        M_icode = json.value("M_icode").toInt();
        M_Cnd = json.value("M_Cnd").toInt();
        M_valA = json.value("M_valA").toInt();
    }
    else
        M_icode = -1;

}

void Fetch::dealWritebackData()
{
    //get W_icode&W_valM;
    QByteArray bytes=clientToWriteback->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("W_icode"))
    {
        W_icode = json.value("W_icode").toInt();
        W_valM = json.value("W_valM").toInt();
    }
    else
        W_icode = -1;
}

void Fetch::dealExecuteData()
{
    QByteArray bytes=clientToExecute->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("E_icode"))
    {
        E_icode = json.value("E_icode").toInt();
        if(json.contains("e_Cnd"))
            e_Cnd = json.value("e_Cnd").toInt();
        else
            E_dstM = json.value("E_dstM").toInt();
    }
    else
        E_icode = -1;
}

void Fetch::dealDecodeData()
{
    //get:D_icode;
    QByteArray bytes=clientToDecode->readAll();
    QJsonObject json=QJsonDocument::fromBinaryData(bytes).object();
    if(json.contains("d_srcA"))
        d_srcA = json.value("d_srcA").toInt();
    else
        d_srcA = -1;
    if(json.contains("d_srcB"))
        d_srcB = json.value("d_srcB").toInt();
    else
        d_srcB = -1;
}

void Fetch::receiveInstr(QString str)
{
    instruction = str;
    switchStrToInt();
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

//获取寄存器的字符串表示，在getInstruction()函数中调用
QString Fetch::getRegStr(int k)
{
    QString s = "";
    switch(k)
    {
        case 0:s += "%eax";
               break;
        case 1:s += "%ecx";
               break;
        case 2:s += "%edx";
               break;
        case 3:s += "%ebx";
               break;
        case 4:s += "%esp";
               break;
        case 5:s += "%ebp";
               break;
        case 6:s += "%esi";
               break;
        case 7:s += "%edi";
               break;
        default:break;
    }
    return s;
}

QString Fetch::getInstruction()
{
    QString s = "";
    switch(instrCode[PC])
    {
         case 0:s += "halt";
                break;
         case 1:s += "nop";
                break;
         case 2:s += "rrmovl ";
                s += getRegStr(f_rA);
                s += ",";
                s += getRegStr(f_rB);
                break;
         case 3:s += "irmovl $";
                s += QString::number(f_valC,10);
                s += ",";
                s += getRegStr(f_rB);
                break;
         case 4:s += "rmmovl ";
                s += getRegStr(f_rA);
                s += ",";
                s += QString::number(f_valC,10);
                s += "("+getRegStr(f_rB)+")";
                break;
         case 5:s += "mrmovl ";
                s += QString::number(f_valC,10);
                s += "("+getRegStr(f_rB)+")";
                s += ",";
                s += getRegStr(f_rA);
                break;
         case 6:switch(f_ifun)
               {
                  case 0:s += "addl ";
                        break;
                  case 1:s += "subl ";
                        break;
                  case 2:s += "andl ";
                        break;
                  case 3:s += "xorl ";
                        break;
                  default:break;
               }
                s += getRegStr(f_rA);
                s += ",";
                s += getRegStr(f_rB);
                break;
         case 7:switch(f_ifun)
               {
                  case 0:s += "jmp ";
                         break;
                  case 1:s += "jle ";
                         break;
                  case 2:s += "jl ";
                         break;
                  case 3:s += "je ";
                         break;
                  case 4:s += "jne ";
                         break;
                  case 5:s += "jge ";
                         break;
                  case 6:s += "jg ";
                         break;
                  default:break;
               }
                s += QString::number(f_valC,10);
                break;
         case 8:s += "call ";
                s += QString::number(f_valC,10);
                break;
         case 9:s += "ret ";
                 break;
         case 10:s += "pushl ";
                s += getRegStr(f_rA);
                break;
         case 11:
                s += "popl ";
                s += getRegStr(f_rA);
                break;
         default:break;
    }
    return s;
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
    else
        f_icode = instrCode[PC];

    if(f_icode == globle::HTL)
        return;
    f_ifun = instrCode[PC+1];
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
    instrString = getInstruction();
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

//需要添加发送到mainwindow的信号
QJsonObject Fetch::dataToMainWindow()
{
    QJsonObject sendData;
    if(f_stat != 0)
        sendData.insert("stat",f_stat);
    else
    {
        sendData.insert("stat",f_stat);
        sendData.insert("PC",PC);
        sendData.insert("predPC", predPC);
        sendData.insert("instruction",instrString);
    }
}
void Fetch::dealClockData()
{
    QString str=QString(clientToClock->readAll());
    if(str=="nextStep")
    {
        //有可能相互阻碍
        clientToWriteback->waitForReadyRead();
        clientToMemory->waitForReadyRead();
        clientToExecute->waitForReadyRead();
        clientToDecode->waitForReadyRead();
        select_PC();
        fetch();
        getInstruction();
        emit sendFromFetch(dataToMainWindow());
        predict_PC();
        sendToDecode(DataToDecode());
        //执行该时钟周期
    }else if(str=="restart")
    {
        PC = 0;
        f_stat = -1;
        instrString = "";
        isRet = false;
        isRisk = false;
        E_icode = -1;
        M_icode = -1;
        W_icode = -1;
    }
}
