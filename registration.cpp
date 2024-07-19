#include "registration.h"
#include "ui_registration.h"
#include "registration.h"
#include "iostream"

QString username;

Registration::Registration(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Registration)
{
    ui->setupUi(this);
}

Registration::~Registration()
{
    delete ui;
}

void Registration::on_pushButton_clicked()
{
    username = ui->lineEdit->text();
    this->close();
}

