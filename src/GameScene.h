#pragma once

#include <QTimer>
#include <QVariantAnimation>
#include <QLinkedList>
#include "MainWindow.h"
#include "Scene.h"
#include "DiaFail.h"


struct dataGameScene
{
    int id;
    int money = 200;
    unsigned long long time = 0;
    int weapon = 400;
    int foodtype = 0;
    int foodnum = 1;
    int gamepoint = 0;
    bool normalState;
    bool running;
    dataGameRegion gameRegion;
};

Q_DECLARE_METATYPE(dataGameScene)

//游戏场景类
class GameScene : public Scene
{
    Q_OBJECT

public:
    explicit GameScene(int _id = 1, QObject* parent = 0);
    explicit GameScene(const dataGameScene& source, QObject* parent = 0);
    QVariant toData();
    virtual ~GameScene();
    void switchGameScene(GameScene* newScene, const QVariant &infm = 0); //转换GameScene
    bool tryBuying(int price); //尝试购买，若金钱不够则返回false
    void toAlienState(int time); //进入入侵状态
    void setState(bool newState); //设置状态

    GameRegion* gameRegion;
    QMediaPlayer* alienPlayer;

    int id; //编号
    int money = 200; //初始金币值
    unsigned long long time = 0; //计时变量
    int weapon = 400; //武器值
    int foodtype = 0; //饲料类型，初始为最低级
    int foodnum = 1; //饲料数量
    int gamepoint = 0; //完成度，达到3即赢得游戏
    bool normalState = true; //true为正常状态,false为入侵状态
    bool running = false; //运行状态

public slots:
    void refresh();
    void pause();
    void start();

protected:
    MenuBar* menuBar;
    QTimer* mainTimer;
    QGraphicsProxyWidget* diaFail;

    void init(int id);
    void win();
    void fail();
};
