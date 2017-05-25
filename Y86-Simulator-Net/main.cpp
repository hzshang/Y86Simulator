#include "maindialog.h"
#include "mainwindow.h"
#include "y86.h"
#include <QApplication>
int eax = 0,ecx = 0,edx = 0,ebx = 0,esp = 0,ebp = 0,esi = 0,edi = 0;//寄存器,按顺序从0～7标记
int AOK = 0,HTL = 1,ADZ = 2,INS = 3;//四种状态指令：AOK(正常),HTL(执行halt指令),ADR(非法地址)和INS(非法指令)
//extern int Cnd;//跳转判断标志
//extern int ZF,SF,OF;//运算条件码Cn：OF:SF:ZF : 000;001;010;011;100;101;110;111;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDialog w;
    Y86 *y86=new Y86();
    MainWindow pipeline;
    w.show();
    QObject::connect(&w,SIGNAL(buildConnect(bool,bool,bool,bool,bool)),y86,SLOT(ready(bool,bool,bool,bool,bool)));
    QObject::connect(y86,SIGNAL(showPipeLine(Y86*)),&pipeline,SLOT(beginpipelineSlot(Y86*)));
    QObject::connect(y86,SIGNAL(showPipeLine(Y86*)),&w,SLOT(on_showPipeline(Y86*)));
    return a.exec();
}
