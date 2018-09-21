#pragma once

#include "MainWindow.h"
#include "Object.h"

class Niko;

struct dataMoney
{
    int id;

    bool alive;
    bool imgReverse;
    double Opacity;
    QPointF pos;
};

Q_DECLARE_METATYPE(dataMoney)

//金币基类
class Money : public Object
{
    Q_OBJECT

public:
    Money(int id, const QPointF& pos, GameRegion* parent = 0);
    Money(int value, const QPointF& pos, frameAnimation* ani, GameRegion* parent = 0);
    virtual ~Money();
    virtual void onleftMousePress(QGraphicsSceneMouseEvent* event);
    virtual void refresh();
    virtual QPainterPath shape() const;
    virtual int type() const;
    static Money* fromData(const QVariant& source, GameRegion* parent = 0);
    virtual QVariant toData();


    const int value; //价值
    int id; //编号

protected:
    const double v = 1;
    static int values[5];
    QPainterPath pathShape;
};

//珍珠类
class Pearl : public Money
{
    Q_OBJECT

public:
    Pearl(const QPointF& pos, GameRegion* parent, Niko* niko);
    virtual void onleftMousePress(QGraphicsSceneMouseEvent* event) override;
    virtual void refresh() override;

protected:
    Niko* niko;
};
