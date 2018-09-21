#include "Object.h"

Object::Object(const QPointF &pos, frameAnimation *ani, GameRegion *parent): QGraphicsObject(parent),
    rectBounding(ani->getRect()), aniDefault(ani)
{
    if(parent)
    {
        setParent(parent);

        //安装事件过滤器
        installSceneEventFilter(parent);
    }

    //pos为Item中心坐标
    setPos(QPointF(pos.x() - rectBounding.width() / 2, pos.y() - rectBounding.height() / 2));

    //初始化翻转坐标变换
    transReverse.translate(rectBounding.width() / 2, 0);
    transReverse.rotate(180, Qt::YAxis);
    transReverse.translate(-rectBounding.width() / 2, 0);

    //初始化帧动画
    ani->setParent(this);
    setDefaultAnimation(ani);
}

Object::~Object()
{
    if(group())
    {
        group()->removeFromGroup(this);
    }
}

QVariant Object::toData()
{
    return QVariant();
}

void Object::onleftMousePress(QGraphicsSceneMouseEvent *event)
{

}

void Object::refresh()
{

}

QRectF Object::boundingRect() const
{
    return rectBounding;
}

void Object::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(imgReverse)
    {
        painter->setTransform(transReverse, true);
    }
    if(!alive)
    {
        painter->setOpacity(Opacity);
    }
    painter->drawImage(rectBounding, *aniDefault->getImg(), aniDefault->getFrame());
}

void Object::setDefaultAnimation(frameAnimation *ani)
{
    //断开原有动画的信号
    if(aniDefault)
    {
        aniDefault->stop();
        disconnect(aniDefault, &frameAnimation::frameChanged, 0, 0);
        disconnect(aniDefault, &frameAnimation::finished, 0, 0);
    }

    aniDefault = ani;
    connect(aniDefault, &frameAnimation::frameChanged, [=](int frame){
        update();
    });

    aniDefault->start();
}

void Object::setReverse()
{
    imgReverse = !imgReverse;
}

void Object::fade(bool living, int duration)
{
    alive = living;
    aniDefault->stop();
    QVariantAnimation *aniFade = new QVariantAnimation(this);
    aniFade->setStartValue(1.0);
    aniFade->setEndValue(0.0);
    aniFade->setDuration(duration);
    aniFade->setEasingCurve(QEasingCurve::OutCubic);
    connect(aniFade, &QVariantAnimation::valueChanged, [=](const QVariant &value){
        Opacity = value.toDouble();
        update();
    });
    connect(aniFade, &QVariantAnimation::finished, this, deleteLater);
    aniFade->start();
}

QPointF Object::centerPos()
{
    QPointF &&p = pos();
    p.rx() += rectBounding.width() / 2;
    p.ry() += rectBounding.height() / 2;
    return p;
}
