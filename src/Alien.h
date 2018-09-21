#pragma once

#include "Object.h"
#include <QPropertyAnimation>

class Fish;
class Money;

struct dataAlien
{
    QVector2D curV; //当前速度(矢量)
    int health; //生命值
    int moneyID; //死后产生的Money种类
    bool alive;
    bool imgReverse;
    double Opacity;
    QPointF pos;

    QString type;
};

Q_DECLARE_METATYPE(dataAlien)


// 怪物基类
class Alien : public Object
{
    Q_OBJECT

public:
    enum State {Swim, Reverse, Eat, Die};

    Alien(const QPointF& pos, frameAnimation *ani, GameRegion* parent = 0);
    virtual QVariant toData();
    virtual ~Alien();
    virtual QPainterPath shape() const;
    virtual int type() const;
    static Alien* fromData(const QVariant& source, GameRegion* parent = 0);

    friend class Itchy;


protected:
    QMap<State, frameAnimation*> aniFrames; //帧动画
    State state = Swim; //当前状态
    randomGenerator random; //随机数生成器
    QPainterPath pathShape; //外形(碰撞检测)
    const qreal maxV = 1; //当前最大速率(标量)

    QVector2D curV; //当前速度(矢量)
    int health; //生命值
    int moneyID; //死后产生的Money种类

    void setAnimations(State state, frameAnimation* ani); //设置动画
    void setState(State newState, Object* food = 0); //设置状态

    //状态切换函数
    virtual void toEat(Object* food);
    void toSwim();
    void toReverse();
    void toDie();
};

class Sylv : public Alien
{
    Q_OBJECT

public:
    Sylv(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    virtual void onleftMousePress(QGraphicsSceneMouseEvent* event);

protected:
    QVariantAnimation aniHited;
};

class Gus : public Alien
{
    Q_OBJECT

public:
    Gus(const QPointF& pos, GameRegion* parent = 0);
    virtual void toEat(Object *food);
    virtual void refresh();
};

class Balrog : public Alien
{
    Q_OBJECT

public:
    Balrog(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    virtual void onleftMousePress(QGraphicsSceneMouseEvent* event);

protected:
    QVariantAnimation aniHited;
};
