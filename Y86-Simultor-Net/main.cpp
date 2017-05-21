#include "maindialog.h"
#include "y86.h"
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainDialog w;
    Y86 *y86=new Y86();
    w.show();
    QObject::connect(&w,SIGNAL(buildConnect(bool,bool,bool,bool,bool)),y86,SLOT(ready(bool,bool,bool,bool,bool)));
    QObject::connect(&w,SIGNAL(cancelConnect()),y86,SLOT(cancel()));
    QObject::connect(y86,SIGNAL(cancelConnectionSginal()),&w,SLOT(on_CancelConnection()));
    return a.exec();
}
