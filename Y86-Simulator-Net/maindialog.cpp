#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    fetch=false;
    decode=false;
    execute=false;
    memory=false;
    writeback=false;
    uncheckColor="background-color:#e7e7e7";
    checkColor="background-color:lightYellow;";
    ui->FButton->setStyleSheet(fetch?checkColor:uncheckColor);
    ui->DButton->setStyleSheet(decode?checkColor:uncheckColor);
    ui->EButton->setStyleSheet(execute?checkColor:uncheckColor);
    ui->MButton->setStyleSheet(memory?checkColor:uncheckColor);
    ui->WButton->setStyleSheet(writeback?checkColor:uncheckColor);
}

MainDialog::~MainDialog()
{
    delete ui;
}


void MainDialog::on_FButton_clicked()
{
    fetch=!fetch;
    ui->FButton->setStyleSheet(fetch?checkColor:uncheckColor);
}

void MainDialog::on_DButton_clicked()
{
    decode=!decode;
    ui->DButton->setStyleSheet(decode?checkColor:uncheckColor);
}

void MainDialog::on_EButton_clicked()
{
    execute=!execute;
    ui->EButton->setStyleSheet(execute?checkColor:uncheckColor);
}
void MainDialog::on_MButton_clicked()
{
    memory=!memory;
    ui->MButton->setStyleSheet(memory?checkColor:uncheckColor);
}

void MainDialog::on_WButton_clicked()
{
    writeback=!writeback;
    ui->WButton->setStyleSheet(writeback?checkColor:uncheckColor);
}

void MainDialog::on_connectButton_clicked()
{
    if(ui->connectButton->text()=="建立连接")
    {
        ui->connectButton->setText("取消连接");
        emit buildConnect(fetch,decode,execute,memory,writeback);
    }else
    {
        ui->connectButton->setText("建立连接");
        emit cancelConnect();
    }
}

void MainDialog::on_CancelConnection()
{
    ui->connectButton->setText("建立连接");
}

void MainDialog::on_showPipeline(Y86 *y86)
{
    this->close();
}
