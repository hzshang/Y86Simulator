#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "y86.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
    void sendInstr(QString);

private slots:
    void beginpipelineSlot(Y86 *);
    void on_openFile_clicked();

    void receiveFromFetch(QJsonObject);

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString instrCode;
    void openFile();
    void readFile();
};

//extern int eax,ecx,edx,ebx,esp,ebp,esi,edi;//寄存器,按顺序从0～7标记
//extern int AOK = 0,HTL = 1,ADZ = 2,INS = 3;//四种状态指令：AOK(正常),HTL(执行halt指令),ADR(非法地址)和INS(非法指令)

#endif // MAINWINDOW_H
