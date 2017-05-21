#include "maindialog.h"
#include "mainwindow.h"
#include "y86.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDialog w;
    Y86 *y86=new Y86();
    MainWindow pipeline;
    w.show();
    QObject::connect(&w,SIGNAL(buildConnect(bool,bool,bool,bool,bool)),y86,SLOT(ready(bool,bool,bool,bool,bool)));
    QObject::connect(&w,SIGNAL(cancelConnect()),y86,SLOT(cancel()));
    QObject::connect(y86,SIGNAL(cancelConnectionSginal()),&w,SLOT(on_CancelConnection()));
    QObject::connect(y86,SIGNAL(showPipeLine(Y86*)),&w,SLOT(on_showPipeline(Y86*)));
    QObject::connect(y86,SIGNAL(showPipeLine(Y86*)),&pipeline,SLOT(beginpipelineSlot(Y86*)));
    return a.exec();
}
