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

private:
    Ui::MainWindow *ui;
    QString fileName;
    QString instrCode;
    void openFile();
    void readFile();
};

#endif // MAINWINDOW_H
