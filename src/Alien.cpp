#include "Alien.h"
#include "Fish.h"
#include "Food.h"
#include "Money.h"

const double maxA = 0.05;

Alien::Alien(const QPointF &pos, frameAnimation *ani, GameRegion *parent):
    Object(pos, ani, parent), random(QTime::currentTime().msec())
{
    if(parent)
    {
        parent->aliens.append(this);
    }
    setZValue(5);
}

QVariant Alien::toData()
{
    dataAlien data;
    Alien* rt = this;
    data.curV  = rt->curV;
    data.health = rt->health;
    data.moneyID = rt->moneyID;
    data.alive = rt->alive;
    data.imgReverse = rt->imgReverse;
    data.Opacity = rt->Opacity;
    data.pos = pos();

    data.type = metaObject()->className();

    QVariant a;
    a.setValue(data);

    return a;
}

Alien::~Alien()
{
    if(parentItem())
    {
        static_cast<GameRegion*>(parentItem())->aliens.removeAll(this);
    }
}

void Alien::setAnimations(Alien::State state, frameAnimation *ani)
{
    aniFrames[state] = ani;
}

void Alien::setState(Alien::State newState, Object* food)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Swim:
        toSwim();
        break;

    case Eat:
        toEat(food);
        break;

    case Reverse:
        toReverse();
        break;

    case Die:
        toDie(); //当场去世
        break;
    }
}

QPainterPath Alien::shape() const
{
    return pathShape;
}

int Alien::type() const
{
    return alien;
}

Alien *Alien::fromData(const QVariant &source, GameRegion *parent)
{
    dataAlien data = source.value<dataAlien>();
    Alien* rt;
    if(data.type == "Sylv")
    {
        rt = new Sylv(data.pos, parent);
    }
    else if(data.type == "Gus")
    {
        rt = new Gus(data.pos, parent);
    }
    else if(data.type == "Balrog")
    {
        rt = new Balrog(data.pos, parent);
    }

    rt->curV = data.curV;
    rt->health = data.health;
    rt->moneyID = data.moneyID;
    rt->alive = data.alive;
    rt->imgReverse = data.imgReverse;
    rt->Opacity = data.Opacity;

    return rt;
}

void Alien::toEat(Object *food)
{

}

void Alien::toSwim()
{
    state = Swim;
    frameAnimation* ani = aniFrames[Swim];
    setDefaultAnimation(ani);
}

void Alien::toReverse()
{
    state = Reverse;
    frameAnimation* ani = aniFrames[Reverse];
    connect(ani, &frameAnimation::finished, [=](){
        setReverse();
        if(imgReverse)
        {
            curV.setX(qAbs(curV.x()));
        }
        else
        {
            curV.setX(-qAbs(curV.x()));
        }
        setState(Swim);
    });
    setDefaultAnimation(ani);
}

void Alien::toDie()
{
    gResourceManager->loadSound("Explode.wav")->play();
    static_cast<GameRegion*>(parentItem())->addToGroup(new Money(moneyID, centerPos(), static_cast<GameRegion*>(parentItem())));
    fade();
}

Sylv::Sylv(const QPointF &pos, GameRegion *parent):
    Alien(pos, new frameAnimation(gResourceManager->loadImage("sylv.png"), 0, 0, 10, 2), parent)
{
    health = 5000;

    pathShape.addRect(38, 9, 94, 139);

    curV.setX(-maxV);
    curV.setY(0);

    moneyID = 3;

    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("sylv.png"), 0, 0, 10, 2, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("sylv.png"), 0, 1, 10, 2, this);
    aniReverse->setLoopCount(1);
    setAnimations(Swim, aniSwim);
    setAnimations(Reverse, aniReverse);
    aniHited.setDuration(500);
}

void Sylv::refresh()
{
    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    if(state == Die)
    {
        //死亡状态
        deleteLater();
        gameRegion->addToGroup(new Money(moneyID, centerPos()));
    }
    else
    {
        //存活状态

        //寻找食物
        QVector2D newV(curV);
        Fish* food = nullptr;
        if(gameRegion->fishes.size())
        {
            food = nearest(this, gameRegion->fishes);
            if(food)
            {
                newV.setX(food->centerPos().x() - centerPos().x());
                newV.setY(food->centerPos().y() - centerPos().y());
                newV = newV.normalized() * maxV;
            }
        }


        //更新运动参数
        if(aniHited.state() == QVariantAnimation::Stopped)
        {
            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
            {
                newV.setX(-newV.x());
            }
            if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
            {
                newV.setY(curV.y() + maxA);
            }
            if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
            {
                newV.setY(curV.y() - maxA);
            }

            //设置掉头动画
            if(curV.x() * newV.x() < 0)
            {
                setState(Reverse);
            }

            //速度限制
            if(newV.length() > maxV)
            {
                newV.normalize();
                newV *= maxV;
            }

            //更新速度和位移
            curV = newV;
            if(state == Reverse)
            {
                moveBy(0, curV.y());
            }
            else
            {
                moveBy(curV.x(), curV.y());
            }
        }

        if(health <= 0)
        {
            setState(Die);
        }

        //消灭最近的鱼
        if(food && collidesWithItem(food))
        {
            gResourceManager->loadSound("VoreEat.wav")->play();
            food->fade();
        }
    }
}

void Sylv::onleftMousePress(QGraphicsSceneMouseEvent* event)
{
    health -= static_cast<GameScene*>(scene())->weapon;
    if(aniHited.state() == QVariantAnimation::Stopped)
    {
        QVector2D dir(rectBounding.center() - event->pos());
        dir.normalize();
        double dx = dir.x();
        double dy = dir.y();
        aniHited.disconnect();
        aniHited.setStartValue(maxV);
        aniHited.setKeyValueAt(0.5, 3 * maxV);
        aniHited.setEndValue(maxV);
        connect(&aniHited, QVariantAnimation::valueChanged, [=](const QVariant &value){
            double v = value.toDouble();
            moveBy(v*dx, v*dy);

            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  dx < 0) || (centerPos().x() > static_cast<GameRegion*>(parentItem())->boundingRect().width() - shape().boundingRect().width() / 2 && dx > 0))
            {
                moveBy(-v*dx, 0);
            }
            if((centerPos().y() < shape().boundingRect().height() / 2 &&  dy < 0) || (centerPos().y() > static_cast<GameRegion*>(parentItem())->boundingRect().height() - shape().boundingRect().height() / 2  && dy > 0))
            {
                moveBy(0, -v*dy);
            }
        });
        aniHited.start();
    }
}

Gus::Gus(const QPointF &pos, GameRegion *parent):
    Alien(pos, new frameAnimation(gResourceManager->loadImage("gus.png"), 0, 0, 10, 3), parent)
{
    pathShape.addEllipse(35, 13, 89, 136);

    health = 5000;

    curV.setX(-maxV);
    curV.setY(0);

    moneyID = 3;

    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("gus.png"), 0, 0, 10, 3, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("gus.png"), 0, 1, 10, 3, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("gus.png"), 0, 2, 10, 3, this);

    aniReverse->setLoopCount(1);
    aniEat->setLoopCount(1);
    setAnimations(Swim, aniSwim);
    setAnimations(Reverse, aniReverse);
    setAnimations(Eat, aniEat);

}

void Gus::toEat(Object *food)
{
    state = Eat;
    if(food->type() == Object::food)
    {
        health -= static_cast<Food*>(food)->nutrition;
        static_cast<Food*>(food)->eated = true;
    }
    else
    {
        health -= static_cast<Fish*>(food)->health;
        static_cast<Fish*>(food)->eated = true;
    }
    food->fade();
    gResourceManager->loadSound("VoreEat.wav")->play();
    frameAnimation* ani = aniFrames[Eat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(Swim);
    });
    setDefaultAnimation(ani);
}

void Gus::refresh()
{
    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    if(state == Die)
    {
        //死亡状态
        deleteLater();
        gameRegion->addToGroup(new Money(moneyID, centerPos()));
    }
    else
    {
        //存活状态

        //更新运动参数
        QVector2D newV(curV);

        //寻找食物(鱼或饲料)
        Object* food = nullptr;
        Fish* food1 = nearest(this, gameRegion->fishes);
        Food* food2 = nearest(this, gameRegion->foods);

        if(food1 == nullptr)
        {
            food = food2;
        }
        else if(food2 == nullptr)
        {
            food = food1;
        }
        else
        {
            QLineF line1(centerPos(), food1->centerPos());
            QLineF line2(centerPos(), food2->centerPos());
            if(line1.length() < line2.length())
            {
                food = food1;
            }
            else
            {
                food = food2;
            }
        }

        if(food)
        {
            newV.setX(food->centerPos().x() - centerPos().x());
            newV.setY(food->centerPos().y() - centerPos().y());
            newV = newV.normalized() * maxV;
        }

        //设定运动边框
        if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
        {
            newV.setX(-newV.x());
        }
        if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
        {
            newV.setY(curV.y() + maxA);
        }
        if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
        {
            newV.setY(curV.y() - maxA);
        }

        qDebug() << health;

        //设置掉头动画
        if(curV.x() * newV.x() < 0)
        {
            setState(Reverse);
        }

        //速度限制
        if(newV.length() > maxV)
        {
            newV.normalize();
            newV *= maxV;
        }

        //更新速度和位移
        curV = newV;
        if(state == Reverse)
        {
            moveBy(0, curV.y());
        }
        else
        {
            moveBy(curV.x(), curV.y());
        }

        //吃掉周围的鱼或饲料
        if(food && collidesWithItem(food))
        {
            setState(Eat, food);
        }

        //更新生命值
        if(health <= 0)
        {
            setState(Die);
        }

    }
}

Balrog::Balrog(const QPointF &pos, GameRegion *parent):
    Alien(pos, new frameAnimation(gResourceManager->loadImage("balrog.png"), 0, 0, 10, 2), parent)
{
    pathShape.addRect(20, 5, 95, 141);

    health = 8000;

    curV.setX(-maxV);
    curV.setY(0);

    moneyID = 3;

    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("balrog.png"), 0, 0, 10, 2, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("balrog.png"), 0, 1, 10, 2, this);
    aniReverse->setLoopCount(1);
    setAnimations(Swim, aniSwim);
    setAnimations(Reverse, aniReverse);
    aniHited.setDuration(500);
}

void Balrog::refresh()
{
    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    if(state == Die)
    {
        //死亡状态
        deleteLater();
        gameRegion->addToGroup(new Money(moneyID, centerPos()));
    }
    else
    {
        //存活状态

        //寻找食物
        QVector2D newV(curV);
        Fish* food = nullptr;
        if(gameRegion->fishes.size() > 0)
        {
            food = nearest(this, gameRegion->fishes);
            if(food)
            {
                newV.setX(food->centerPos().x() - centerPos().x());
                newV.setY(food->centerPos().y() - centerPos().y());
                newV = newV.normalized() * maxV;
            }
        }


        //更新运动参数
        if(aniHited.state() == QVariantAnimation::Stopped)
        {
            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
            {
                newV.setX(-newV.x());
            }
            if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
            {
                newV.setY(curV.y() + maxA);
            }
            if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
            {
                newV.setY(curV.y() - maxA);
            }

            //设置掉头动画
            if(curV.x() * newV.x() < 0)
            {
                setState(Reverse);
            }

            //速度限制
            if(newV.length() > maxV)
            {
                newV.normalize();
                newV *= maxV;
            }

            //更新速度和位移
            curV = newV;
            if(state == Reverse)
            {
                moveBy(0, curV.y());
            }
            else
            {
                moveBy(curV.x(), curV.y());
            }
        }

        if(health <= 0)
        {
            setState(Die);
        }

        //消灭周围的鱼
        if(food && collidesWithItem(food))
        {
            gResourceManager->loadSound("VoreEat.wav")->play();
            food->fade();
        }
    }
}

void Balrog::onleftMousePress(QGraphicsSceneMouseEvent *event)
{
    health -= static_cast<GameScene*>(scene())->weapon;
    if(aniHited.state() == QVariantAnimation::Stopped)
    {
        QVector2D dir(rectBounding.center() - event->pos());
        dir.normalize();
        double dx = dir.x();
        double dy = dir.y();
        aniHited.disconnect();
        aniHited.setStartValue(maxV);
        aniHited.setKeyValueAt(0.5, 3 * maxV);
        aniHited.setEndValue(maxV);
        connect(&aniHited, QVariantAnimation::valueChanged, [=](const QVariant &value){
            double v = value.toDouble();
            moveBy(v*dx, v*dy);

            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  dx < 0) || (centerPos().x() > static_cast<GameRegion*>(parentItem())->boundingRect().width() - shape().boundingRect().width() / 2 && dx > 0))
            {
                moveBy(-v*dx, 0);
            }
            if((centerPos().y() < shape().boundingRect().height() / 2 &&  dy < 0) || (centerPos().y() > static_cast<GameRegion*>(parentItem())->boundingRect().height() - shape().boundingRect().height() / 2  && dy > 0))
            {
                moveBy(0, -v*dy);
            }
        });
        aniHited.start();
    }
}
