#include "Money.h"
#include "Pet.h"

int Money::values[5] = {15, 35, 100 ,200, 2000};

Money::Money(int id, const QPointF &pos, GameRegion *parent): value(values[id]), id(id),
    Object(pos, new frameAnimation(gResourceManager->loadImage("money.png"), 0, id, 10, 6), parent)
{
    if(parent)
    {
        parent->moneys.append(this);
    }
    setZValue(6);

    //初始化边框
    switch(id)
    {
    case 0:
        pathShape.addEllipse(20, 20, 32, 32);
        break;

    case 1:
        pathShape.addEllipse(20, 20, 32, 32);
        break;

    case 2:
        break;

    case 3:
        pathShape.addEllipse(20, 18, 33, 33);
        break;

    case 4:
        pathShape.addEllipse(15, 12, 45, 50);
        break;

    }
}

Money::Money(int value, const QPointF &pos, frameAnimation* ani, GameRegion *parent):
    Object(pos, ani, parent), value(value)
{
    if(parent)
    {
        parent->moneys.append(this);
    }
    setZValue(2);
}

Money::~Money()
{
    if(parentItem())
    {
        static_cast<GameRegion*>(parentItem())->moneys.removeAll(this);
    }
}

void Money::onleftMousePress(QGraphicsSceneMouseEvent *event)
{
    static_cast<GameScene*>(scene())->money += value;

    alive = false;

    QVariantAnimation* aniFly = new QVariantAnimation;
    aniFly->setStartValue(pos());
    aniFly->setEndValue(parentItem()->mapFromScene(QPointF(510, 30)));
    aniFly->setDuration(400);

    connect(aniFly, &QVariantAnimation::valueChanged, [=](const QVariant& value){
        setPos(value.toPointF());
    });
    connect(aniFly, &QVariantAnimation::finished, [=](){
        aniFly->deleteLater();
        deleteLater();
    });
    gResourceManager->loadSound("Getmoney.wav")->play();
    aniFly->start();
}

void Money::refresh()
{
    if(alive)
    {
        moveBy(0, v);
        if(centerPos().y() > parentItem()->boundingRect().height())
            fade();
    }
}

QPainterPath Money::shape() const
{
    return pathShape;
}

int Money::type() const
{
    return money;
}

Money *Money::fromData(const QVariant &source, GameRegion* parent)
{
    Money* rt;
    dataMoney data = source.value<dataMoney>();
    rt = new Money(data.id, data.pos, parent);
    rt->alive = data.alive;
    rt->imgReverse = data.imgReverse;
    rt->Opacity = data.Opacity;
    return rt;
}

QVariant Money::toData()
{
    dataMoney data;
    Money* rt = this;
    data.id = rt->id;

    data.alive = rt->alive;
    data.imgReverse = rt->imgReverse;
    data.Opacity = rt->Opacity;
    data.pos = pos();

    QVariant a;
    a.setValue(data);

    return a;
}

Pearl::Pearl(const QPointF &pos, GameRegion *parent, Niko *niko): niko(niko),
    Money(250, pos, new frameAnimation(gResourceManager->loadImage("pearl.png"), 0, 0, 1, 1), parent)
{
    pathShape.addEllipse(18, 19, 36, 36);
}

void Pearl::onleftMousePress(QGraphicsSceneMouseEvent *event)
{
    static_cast<GameScene*>(scene())->money += value;
    niko->picked();

    QVariantAnimation* aniFly = new QVariantAnimation;
    aniFly->setStartValue(pos());
    aniFly->setEndValue(parentItem()->mapFromScene(QPointF(510, 30)));
    aniFly->setDuration(400);

    connect(aniFly, &QVariantAnimation::valueChanged, [=](const QVariant& value){
        setPos(value.toPointF());
    });
    connect(aniFly, &QVariantAnimation::finished, [=](){
        aniFly->deleteLater();
        deleteLater();
    });
    gResourceManager->loadSound("Getmoney.wav")->play();
    aniFly->start();
}

void Pearl::refresh()
{

}
