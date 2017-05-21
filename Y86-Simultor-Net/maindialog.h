#ifndef MAINDIALOG_H
#define MAINDIALOG_H

#include <QDialog>
#include "y86.h"
namespace Ui {
class MainDialog;
}

class MainDialog : public QDialog
{
    Q_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

private slots:
    void on_MButton_clicked();

    void on_FButton_clicked();

    void on_DButton_clicked();

    void on_EButton_clicked();

    void on_WButton_clicked();

    void on_connectButton_clicked();
    void on_CancelConnection();
signals:
    void buildConnect(bool,bool,bool,bool,bool);
    void cancelConnect();
private:
    Ui::MainDialog *ui;
    bool fetch;
    bool decode;
    bool execute;
    bool memory;
    bool writeback;
    QString checkColor;
    QString uncheckColor;
};

#endif // MAINDIALOG_H
