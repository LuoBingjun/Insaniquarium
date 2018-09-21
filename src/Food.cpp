#include "Food.h"

int Food::nutritions[3] = {600, 700, 800};

Food::Food(int id, const QPointF &pos, GameRegion *parent): nutrition(nutritions[id]), id(id),
    Object(pos, new frameAnimation(gResourceManager->loadImage("food.png"), 0, id, 10, 5), parent)
{
    if(parent)
    {
        parent->foods.append(this);
    }
    setZValue(1);

    aniDefault->setDuration(1000);

    //初始化边框
    switch(id)
    {
    case 0:
        pathShape.addEllipse(5, 11, 30, 18);
        break;

    case 1:
        pathShape.addEllipse(9, 9, 22, 22);
        break;

    case 2:
        pathShape.addEllipse(9, 9, 22, 22);
        break;
    }
}

Food *Food::fromData(const QVariant &source, GameRegion *parent)
{
    dataFood data = source.value<dataFood>();
    Food* rt = new Food(data.id, data.pos, parent);
    rt->eated = data.eated;
    rt->alive = data.alive;
    rt->imgReverse = data.imgReverse;
    rt->Opacity = data.Opacity;
}

Food::~Food()
{
    if(parentItem())
    {
        static_cast<GameRegion*>(parentItem())->foods.removeAll(this);
    }
}

void Food::refresh()
{
    if(centerPos().y() > parentItem()->boundingRect().height())
        fade();
    else
        moveBy(0, v);
}

QPainterPath Food::shape() const
{
    return pathShape;
}

int Food::type() const
{
    return food;
}

QVariant Food::toData()
{
    dataFood data;
    Food* rt = this;
    data.eated = rt->eated;
    data.id = rt->id;

    data.alive = rt->alive;
    data.imgReverse = rt->imgReverse;
    data.Opacity = rt->Opacity;
    data.pos = pos();

    QVariant a;
    a.setValue(data);

    return a;
}
