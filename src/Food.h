#pragma once

#include "Object.h"

struct dataFood
{
    int id;
    bool eated;

    bool alive;
    bool imgReverse;
    double Opacity;
    QPointF pos;
};

Q_DECLARE_METATYPE(dataFood)

//饲料基类
class Food : public Object
{
    Q_OBJECT

public:
    Food(int id, const QPointF& pos, GameRegion* parent = 0);
    static Food* fromData(const QVariant& source, GameRegion* parent = 0);
    virtual ~Food();
    virtual void refresh();
    virtual QPainterPath shape() const;
    virtual int type() const;
    virtual QVariant toData();

    const int nutrition; //营养值
    bool eated = false; //是否被吃掉

protected:
    int id;
    double v = 1;
    static int nutritions[3];
    QPainterPath pathShape;
};


