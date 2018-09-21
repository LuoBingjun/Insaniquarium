#ifndef DIAOPTIONS_H
#define DIAOPTIONS_H

#include <QWidget>

// 设置对话框
namespace Ui {
class DiaOptions;
}

class DiaOptions : public QWidget
{
    Q_OBJECT

public:
    explicit DiaOptions(QWidget *parent = 0);
    ~DiaOptions();

signals:
    void BacktoMenu();
    void Back();
    void Ok();

private slots:
    void on_pushButton_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

private:
    Ui::DiaOptions *ui;
};

#endif // DIAOPTIONS_H
