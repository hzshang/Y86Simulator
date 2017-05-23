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
private slots:
    void beginpipelineSlot(Y86 *);
    void on_openFile_clicked();

private:
    Ui::MainWindow *ui;
    QString fileName;
    void openFile();
    void readfile();
};

#endif // MAINWINDOW_H
