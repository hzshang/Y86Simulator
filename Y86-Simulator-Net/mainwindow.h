#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "y86.h"
#include <QLineEdit>
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
    void PipelineRun();
    void PipelineStop();
    void PipelineStep();
    void PipelineRestart();

private slots:
    void beginpipelineSlot(Y86 *);
    void on_openFile_clicked();

    void receiveFromFetch(QJsonObject);
    void receiveFromDecode(QJsonObject);
    void receiveFromExecute(QJsonObject);
    void receiveFromMemory(QJsonObject);
    void receiveFromWriteback(QJsonObject);
    void receiveCC(QJsonObject);
    void receiveClear(bool);
    void on_run_clicked();
    void on_next_clicked();
    void on_restart_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString instrCode;
    void openFile();
    void readFile();
    void setLine(QLineEdit *l,QString str);
    void writeReg(QString dst,QString val);
    Y86 *wy86;
};
#endif // MAINWINDOW_H
