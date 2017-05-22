#include "maindialog.h"
#include "ui_maindialog.h"

MainDialog::MainDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MainDialog)
{
    ui->setupUi(this);
    uncheckColor="background-color:#e7e7e7";
    checkColor="background-color:lightYellow;";
    init();

}
void MainDialog::init()
{
    fetch=false;
    decode=false;
    execute=false;
    memory=false;
    writeback=false;
    ui->FButton->setStyleSheet(uncheckColor);
    ui->DButton->setStyleSheet(uncheckColor);
    ui->EButton->setStyleSheet(uncheckColor);
    ui->MButton->setStyleSheet(uncheckColor);
    ui->WButton->setStyleSheet(uncheckColor);
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
    if(fetch||memory||decode||execute||writeback)
    {
        ui->connectButton->setText("正在连接...");
        ui->connectButton->setEnabled(false);
        ui->FButton->setEnabled(false);
        ui->DButton->setEnabled(false);
        ui->EButton->setEnabled(false);
        ui->MButton->setEnabled(false);
        ui->WButton->setEnabled(false);
        emit buildConnect(fetch,decode,execute,memory,writeback);
    }else{
        QMessageBox::warning(NULL,"Warning",QString("请选择至少一个阶段！"),QMessageBox::Ok);
    }
}

void MainDialog::on_showPipeline(Y86 *y86)
{
    this->close();
}
