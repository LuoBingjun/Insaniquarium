#include "DiaFail.h"
#include "ui_DiaFail.h"

DiaFail::DiaFail(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiaFail)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setAttribute(Qt::WA_TranslucentBackground); //背景透明
}

DiaFail::~DiaFail()
{
    delete ui;
}

void DiaFail::on_pushButton_clicked()
{
    emit clicked();
    close();
}
