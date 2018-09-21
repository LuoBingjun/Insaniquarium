#ifndef DIAFAIL_H
#define DIAFAIL_H

#include <QWidget>

// 失败对话框
namespace Ui {
class DiaFail;
}

class DiaFail : public QWidget
{
    Q_OBJECT

public:
    explicit DiaFail(QWidget *parent = 0);
    ~DiaFail();

signals:
    void clicked();

private slots:
    void on_pushButton_clicked();

private:
    Ui::DiaFail *ui;
};

#endif // DIAFAIL_H
