#pragma once

#include <QState>
#include <QGraphicsItemGroup>
#include "MainWindow.h"
#include "Object.h"
#include "GameScene.h"

class Food;
class Money;

struct dataFish
{
    QVector2D curV;
    qreal maxV;
    int health;
    int age;
    int timeToEat;
    bool eated;
    int timeToProduce;
    bool toBeKing;

    bool alive;
    bool imgReverse;
    double Opacity;
    QPointF pos;

    QString type;
};

Q_DECLARE_METATYPE(dataFish)

// 鱼类基类
class Fish : public Object
{
    Q_OBJECT

public:
    enum State {Swim, Eat, Reverse, HungrySwim, HungryEat, HungryReverse, Die};

    Fish(const QPointF& pos, frameAnimation *ani, GameRegion* parent = 0);
    virtual ~Fish();
    void setAnimations(State state, frameAnimation* ani); //设置状态
    void appear(); //淡入效果函数
    virtual void setState(State newState, Object* food = 0); //设置状态
    virtual QPainterPath shape() const;
    virtual int type() const;
    static Fish* fromData(const QVariant& source, GameRegion* parent = 0);
    virtual QVariant toData();

    friend class Gus;

protected:
    QMap<State, frameAnimation*> aniFrames; //帧动画
    randomGenerator random; //随机数生成器
    QPainterPath pathShape; //外形(碰撞检测)
    const static int cdEat = 200; //摄食冷却时间


    State state = Swim; //当前状态
    QVector2D curV; //当前速度(矢量)
    qreal maxV; //当前最大速率(标量)
    int health = 800; //生命值(0~1000):health<=400时为饥饿状态,health=0时死亡
    int age = 0; //年龄值
    int timeToEat = cdEat; //摄食剩余时间
    bool eated = false; //是否被吃掉
    int timeToProduce = 250; //产出剩余时间
    bool toBeKing = false;

    virtual void toEat(Object *_food);
    virtual void toHungryEat(Object *_food);

    //状态切换函数
    void toSwim();
    void toReverse();
    void toHungrySwim();
    void toHungryReverse();
    void toDie();

    //更新函数
    void updateMoveModel();
    void updateStateModel();
    void updateDieModel();
};

class SmallGuppy : public Fish
{
    Q_OBJECT

public:
    SmallGuppy(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    friend class MediumGuppy;
    friend class Carnivore;
};

class MediumGuppy : public Fish
{
    Q_OBJECT

public:
    MediumGuppy(SmallGuppy* source, GameRegion* parent = 0);
    virtual void refresh();

    friend class BigGuppy;

protected:
    const static int moneyID = 0; //产出钱币的ID
    const static int cdProduce = 250; //产出冷却时间
};

class BigGuppy : public Fish
{
    Q_OBJECT

public:
    BigGuppy(MediumGuppy* source, GameRegion* parent = 0);
    virtual void refresh();
    friend class KingGuppy;

protected:
    const static int moneyID = 1; //产出钱币的ID
    const static int cdProduce = 500; //产出冷却时间
};

class KingGuppy : public Fish
{
    Q_OBJECT

public:
    KingGuppy(BigGuppy* source, GameRegion* parent = 0);
    virtual void refresh();

protected:
    const static int moneyID = 3; //产出钱币的ID
    const static int cdProduce = 500; //产出冷却时间
};

class Carnivore : public Fish
{
    Q_OBJECT

public:
    Carnivore(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();

    friend class UltraVore;

protected:
    const static int moneyID = 3; //产出钱币的ID
    const static int cdProduce = 750; //产出冷却时间

    virtual void toEat(Object* _food) override;
    virtual void toHungryEat(Object *_food) override;
};

class UltraVore : public Fish
{
    Q_OBJECT

public:
    UltraVore(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    virtual void setState(State newState, Object* food = 0) override;

protected:
    const static int moneyID = 4; //产出钱币的ID
    const static int cdProduce = 1000; //产出冷却时间

    virtual void toEat(Object *_food) override;
};

class Breeder : public Fish
{
    Q_OBJECT

public:
    Breeder(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    friend class BigBreeder;

protected:
    const static int cdProduce = 2000; //产出冷却时间
};

class BigBreeder : public Fish
{
    Q_OBJECT

public:
    BigBreeder(Breeder* source, GameRegion* parent = 0);
    virtual void refresh();

protected:
    const static int cdProduce = 1000; //产出冷却时间
};
