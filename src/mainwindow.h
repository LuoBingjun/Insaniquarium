#pragma once

#include <QWidget>
#include <QGraphicsView>
#include <QSettings>
#include <QDebug>
#include <QPushButton>
#include <QCloseEvent>
#include "utils.h"

class GameScene;

//主窗口类
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    QRect getViewRect() const; //计算MainView的位置参数
    void loadSettings();
    ~MainWindow();

protected:
    virtual void resizeEvent(QResizeEvent *event);
};

//主视图类
class MainView : public QGraphicsView
{
    Q_OBJECT

public:
    explicit MainView(MainWindow *parent = 0);
    MainWindow *getMainWindow() const; //获得主窗口的指针
    void toGameScene();
    void setGameScene(GameScene* newGame);

    QPushButton* btnToWin;
    QPushButton* btnToAdd;
    GameScene* curGame = nullptr;

protected:
    virtual void resizeEvent(QResizeEvent *event);

private:
    MainWindow *mainwindow;
};

extern MainView* gMainView; //全局主视图指针
extern QSettings* gSettings; //全局设置指针
extern ResourceManager* gResourceManager; //全局资源管理器指针
