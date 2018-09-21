#pragma once

#include "Object.h"

class Money;
class Pearl;

struct dataPet
{
    QVector2D curV;
    double curVf;

    bool alive;
    bool imgReverse;
    double Opacity;
    QPointF pos;

    QString type;
};

Q_DECLARE_METATYPE(dataPet)

//宠物基类
class Pet : public Object
{
    Q_OBJECT

public:
    Pet(const QPointF& pos, frameAnimation *ani, GameRegion* parent = 0);
    virtual ~Pet();
    virtual void refresh() = 0;
    virtual QPainterPath shape() const;
    virtual int type() const;

    static Pet* fromData(const QVariant& source, GameRegion* parent = 0);
    virtual QVariant toData();

protected:
    QPainterPath pathShape; //外形(碰撞检测)
    QVector2D curV; //当前速度
    double curVf = 1;
};

class Stinky : public Pet
{
    Q_OBJECT

public:
    enum State {Crawl, Reverse, Retracted};

    Stinky(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();

protected:
    QMap<State, frameAnimation*> aniFrames;
    randomGenerator random; //随机数生成器
    State state = Crawl;
    const int maxV = 2;

    void setState(State newState);
    void toCrawl();
    void toReverse();
    void toRetracted();

};


class Niko : public Pet
{
    Q_OBJECT

public:
    enum State {Off, Open, On, Close};

    Niko(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    void picked();

protected:
    QMap<State, frameAnimation*> aniFrames;
    State state = Off;
    const int cdTime = 500;
    int timer = cdTime;
    Pearl* m_Pearl = nullptr;

    void open();
    void close();
};

class Itchy : public Pet
{
    Q_OBJECT

public:
    enum State {Swim, Reverse};

    Itchy(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    void setState(State newState);

protected:
    QMap<State, frameAnimation*> aniFrames;
    randomGenerator random; //随机数生成器
    State state = Swim;
    const int maxV = 1; //最大速度

    void toSwim();
    void toReverse();
};

class Vert : public Pet
{
    Q_OBJECT

public:
    enum State {Swim, Reverse};

    Vert(const QPointF& pos, GameRegion* parent = 0);
    virtual void refresh();
    void setState(State newState);

protected:
    QMap<State, frameAnimation*> aniFrames;
    randomGenerator random; //随机数生成器
    State state = Swim;
    const static int maxV = 1; //最大速度
    const static int cdTime = 500;
    int timer = cdTime;

    void toSwim();
    void toReverse();

};
