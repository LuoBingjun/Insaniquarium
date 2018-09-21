#include "DiaOptions.h"
#include "ui_DiaOptions.h"
#include "MainWindow.h"

DiaOptions::DiaOptions(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DiaOptions)
{
    ui->setupUi(this);

    setWindowFlags(Qt::FramelessWindowHint); //无边框
    setAttribute(Qt::WA_TranslucentBackground); //背景透明

    ui->lineEdit->setText(gSettings->value("User/name").toString());
    ui->horizontalSlider->setValue(gSettings->value("General/volume").toInt());
    ui->checkBox->setChecked(gSettings->value("General/fullscreen").toBool());
}

DiaOptions::~DiaOptions()
{
    delete ui;
}

void DiaOptions::on_pushButton_clicked()
{
    gSettings->setValue("User/name", ui->lineEdit->text());
    gSettings->setValue("General/fullscreen", ui->checkBox->isChecked());
    gSettings->setValue("General/volume", ui->horizontalSlider->value());
    emit Ok();
    close();
}

void DiaOptions::on_pushButton_2_clicked()
{
    ui->lineEdit->setText(gSettings->value("User/name").toString());
    ui->horizontalSlider->setValue(gSettings->value("General/volume").toInt());
    ui->checkBox->setChecked(gSettings->value("General/fullscreen").toBool());
    emit Back();
    close();
}

void DiaOptions::on_pushButton_3_clicked()
{
    ui->lineEdit->setText(gSettings->value("User/name").toString());
    ui->horizontalSlider->setValue(gSettings->value("General/volume").toInt());
    ui->checkBox->setChecked(gSettings->value("General/fullscreen").toBool());
    emit BacktoMenu();
    close();
}
