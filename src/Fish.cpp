#include "Fish.h"
#include "Food.h"
#include "Money.h"
#include "Alien.h"

const double maxA = 0.02; //最大加速度

//速度关于生命值表达式的参数
//maxV = b / (health + a) + c
const double a = 300;
const double b = 900;
const double c = 1;

Fish::Fish(const QPointF &pos, frameAnimation *ani, GameRegion *parent):
    Object(pos, ani, parent), random(QTime::currentTime().msec())
{
    if(parent)
    {
        parent->fishes.append(this);
    }
    setZValue(3);
}

Fish::~Fish()
{
    if(parentItem())
    {
        static_cast<GameRegion*>(parentItem())->fishes.removeAll(this);
    }
}

void Fish::setAnimations(Fish::State state, frameAnimation *ani)
{
    aniFrames[state] = ani;
}

void Fish::appear()
{
    alive = false;
    QVariantAnimation* aniAppear = new QVariantAnimation;
    aniAppear->setStartValue(pos());
    aniAppear->setEndValue(QPointF(pos().x(), pos().y() + 100));
    aniAppear->setDuration(400);
    aniAppear->setEasingCurve(QEasingCurve::OutQuad);

    connect(aniAppear, &QVariantAnimation::valueChanged, [=](const QVariant &value){
        setPos(value.toPointF());
        update();
    });

    connect(aniAppear, &QVariantAnimation::finished, [=](){
        alive = true;
        aniAppear->deleteLater();
    });
    aniAppear->start();
}

void Fish::setState(Fish::State newState, Object *food)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Swim:
        if(aniDefault->state() == QTimeLine::NotRunning || state == HungrySwim)
        {
            toSwim();
        }
        else
        {
            connect(aniDefault, &frameAnimation::finished, [=](){
                toSwim();
            });
        }
        break;

    case Eat:
        toEat(food);
        break;

    case Reverse:
        toReverse();
        break;

    case HungrySwim:
        if(aniDefault->state() == QTimeLine::NotRunning || state == Swim)
        {
            toHungrySwim();
        }
        else
        {
            connect(aniDefault, &frameAnimation::finished, [=](){
                toHungrySwim();
            });
        }
        break;

    case HungryEat:
        toHungryEat(food);
        break;

    case HungryReverse:
        toHungryReverse();
        break;

    case Die:
        toDie(); //当场去世
        break;
    }
}

QPainterPath Fish::shape() const
{
    return pathShape;
}

int Fish::type() const
{
    return fish;
}

Fish *Fish::fromData(const QVariant &source, GameRegion *parent)
{
    dataFish data = source.value<dataFish>();
    Fish* rt;
    if(data.type == "SmallGuppy")
    {
        rt = new SmallGuppy(data.pos, parent);
    }
    else if(data.type == "MediumGuppy")
    {
        SmallGuppy* old = new SmallGuppy(data.pos, parent);
        rt = new MediumGuppy(old, parent);
        delete old;
    }
    else if(data.type == "BigGuppy")
    {
        SmallGuppy* old0 = new SmallGuppy(data.pos, parent);
        MediumGuppy* old1 = new MediumGuppy(old0, parent);
        rt = new BigGuppy(old1, parent);
        delete old0;
        delete old1;
    }
    else if(data.type == "KingGuppy")
    {
        SmallGuppy* old0 = new SmallGuppy(data.pos, parent);
        MediumGuppy* old1 = new MediumGuppy(old0, parent);
        BigGuppy* old2 = new BigGuppy(old1, parent);
        rt = new KingGuppy(old2, parent);
        delete old0;
        delete old1;
        delete old2;
    }
    else if(data.type == "Carnivore")
    {
        rt = new Carnivore(data.pos, parent);
    }
    else if(data.type == "UltraVore")
    {
        rt = new UltraVore(data.pos, parent);
    }
    else if(data.type == "Breeder")
    {
        rt = new Breeder(data.pos, parent);
    }
    else if(data.type == "BigBreeder")
    {
        Breeder* old = new Breeder(data.pos, parent);
        rt = new BigBreeder(old, parent);
        delete old;
    }

    rt->curV = data.curV;
    rt->maxV = data.maxV;
    rt->health = data.health;
    rt->age = data.age;
    rt->timeToEat = data.timeToEat;
    rt->eated = data.eated;
    rt->timeToProduce = data.timeToProduce;
    rt->toBeKing = data.toBeKing;

    rt->alive = data.alive;
    rt->imgReverse = data.imgReverse;
    rt->Opacity = data.Opacity;

    return rt;
}

QVariant Fish::toData()
{
    dataFish data;
    Fish* rt = this;
    data.curV = rt->curV;
    data.maxV = rt->maxV;
    data.health = rt->health;
    data.age = rt->age;
    data.timeToEat = rt->timeToEat;
    data.eated = rt->eated;
    data.timeToProduce = rt->timeToProduce;
    data.toBeKing = rt->toBeKing;

    data.alive = rt->alive;
    data.imgReverse = rt->imgReverse;
    data.Opacity = rt->Opacity;
    data.pos = pos();

    data.type = metaObject()->className();

    QVariant a;
    a.setValue(data);

    return a;
}

void Fish::toEat(Object *_food)
{
    Food* food = static_cast<Food*>(_food);
    state = Eat;
    food->eated = true;
    health += food->nutrition;
    timeToEat = cdEat;
    frameAnimation* ani = aniFrames[Eat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(Swim);
    });
    setDefaultAnimation(ani);
    food->fade(true, 250);
}

void Fish::toSwim()
{
    state = Swim;
    frameAnimation* ani = aniFrames[Swim];
    setDefaultAnimation(ani);
}

void Fish::toReverse()
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

void Fish::toHungrySwim()
{
    state = HungrySwim;
    frameAnimation* ani = aniFrames[HungrySwim];
    setDefaultAnimation(ani);
}

void Fish::toHungryEat(Object *_food)
{
    Food* food = static_cast<Food*>(_food);
    state = HungryEat;
    food->eated = true;
    health += food->nutrition;
    timeToEat = cdEat;
    frameAnimation* ani = aniFrames[HungryEat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(HungrySwim);
    });
    setDefaultAnimation(ani);
    food->fade(true, 250);
}

void Fish::toHungryReverse()
{
    state = HungryReverse;
    frameAnimation* ani = aniFrames[HungryReverse];
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
        setState(HungrySwim);
    });
    setDefaultAnimation(ani);
}

void Fish::toDie()
{
    state = Die;
    frameAnimation* ani = aniFrames[Die];
    setDefaultAnimation(ani);
}

void Fish::updateMoveModel()
{
    //确定最大速度
    maxV = b / (health + a) + c;

    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    //确定捕食对象
    Food* food = nullptr;
    QVector2D newV(curV.x() + random.bounded(-maxA, maxA), curV.y() + random.bounded(-maxA, 0.9 * maxA));

    if(static_cast<GameScene*>(scene())->normalState || gameRegion->battle)
    {
        if((state == HungrySwim || state == HungryReverse || timeToEat == 0)
                && (gameRegion->foods.size()))
        {
            food = nearest(this, gameRegion->foods);
            if(food)
            {
                if(food->eated)
                {
                    food = nullptr;
                }
                else
                {
                    newV.setX(food->centerPos().x() - centerPos().x());
                    newV.setY(food->centerPos().y() - centerPos().y());
                    newV = newV.normalized() * maxV;
                }
            }
        }
    }

    //设定运动边框
    if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
    {
        newV.setX(-newV.x());
    }
    if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
    {
        newV.setY(curV.y() + 5 * maxA);
    }
    if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
    {
        newV.setY(curV.y() - 5 * maxA);
    }

    //设置掉头动画
    if(curV.x() * newV.x() < 0)
    {
        if(state == Swim || state == Eat)
        {
            setState(Reverse);
        }
        if(state == HungrySwim || state == HungryEat)
        {
            setState(HungryReverse);
        }
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

    //更新食物
    if(food && collidesWithItem(food))
    {
        if(state == Swim)
            setState(Eat, food);
        if(state == HungrySwim)
            setState(HungryEat, food);
    }
}

void Fish::updateStateModel()
{
    //更新生理参数
    age++;
    health--;
    if(timeToEat > 0)
    {
        timeToEat--;
    }

    //更新状态
    if(health >= 400)
    {
        if(state == HungryEat || state == HungrySwim || state == HungryReverse)
            setState(Swim);
    }
    else if(health > 0)
    {
        if(state == Eat || state == Swim || state == Reverse)
            setState(HungrySwim);
    }
    else
    {
        setState(Die);
    }

    /*
    qDebug() << health;
    qDebug() << age;
    qDebug() << '\n';
    */
}

void Fish::updateDieModel()
{
    moveBy(0, maxV);
    if(centerPos().y() >= group()->boundingRect().height())
    {
        fade();
    }
}

SmallGuppy::SmallGuppy(const QPointF& pos, GameRegion *parent) :
    Fish(pos, new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 0, 10, 5), parent)
{
    //初始化边框
    pathShape.addEllipse(24, 30, 21, 21);

    //初始化参数
    maxV = b / (health + a) + c;
    curV.setX(-maxV);
    curV.setY(0);

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 0, 10, 5, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("smalleat.png"), 0, 0, 10, 5, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("smallturn.png"), 0, 0, 10, 5, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungryswim.png"), 0, 0, 10, 5, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungryeat.png"), 0, 0, 10, 5, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungryturn.png"), 0, 0, 10, 5, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("smalldie.png"), 0, 0, 10, 5, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void SmallGuppy::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现成长过程
        if(state == Swim && age >= 500 && health >= 600)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new MediumGuppy(this, gameRegion));
            gResourceManager->loadSound("Grow.wav")->play();
            deleteLater();
        }
    }
}

MediumGuppy::MediumGuppy(SmallGuppy *source, GameRegion *parent):
    Fish(source->centerPos(), new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 1, 10, 5), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(18, 26, 30, 30);

    //复制运动参数
    curV = source->curV;
    maxV = source->maxV;
    imgReverse = source->imgReverse;

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 1, 10, 5, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("smalleat.png"), 0, 1, 10, 5, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("smallturn.png"), 0, 1, 10, 5, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungryswim.png"), 0, 1, 10, 5, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungryeat.png"), 0, 1, 10, 5, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungryturn.png"), 0, 1, 10, 5, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("smalldie.png"), 0, 1, 10, 5, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void MediumGuppy::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 600)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new Money(moneyID, centerPos(), gameRegion));
            health -= 50;
            timeToProduce = cdProduce;
        }

        //实现成长过程
        if(state == Swim && age >= 1000 && health >= 1000)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new BigGuppy(this, gameRegion));
            gResourceManager->loadSound("Grow.wav")->play();
            deleteLater();
        }
    }
}

BigGuppy::BigGuppy(MediumGuppy *source, GameRegion *parent):
    Fish(source->centerPos(), new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 2, 10, 5), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(18, 26, 30, 30);

    //复制运动参数
    curV = source->curV;
    maxV = source->maxV;
    imgReverse = source->imgReverse;

    //随机变为GuppyKing
    toBeKing = (random.bounded(-1.0, 1.0) > 0);

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 2, 10, 5, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("smalleat.png"), 0, 2, 10, 5, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("smallturn.png"), 0, 2, 10, 5, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungryswim.png"), 0, 2, 10, 5, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungryeat.png"), 0, 2, 10, 5, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungryturn.png"), 0, 2, 10, 5, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("smalldie.png"), 0, 2, 10, 5, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void BigGuppy::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 600)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new Money(moneyID, centerPos(), gameRegion));
            health -= 100;
            timeToProduce = cdProduce;
        }

        //实现成长过程
        if(toBeKing && state == Swim && age >= 2000 && health >= 1000)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new KingGuppy(this, gameRegion));
            gResourceManager->loadSound("Grow.wav")->play();
            deleteLater();
        }
    }
}

KingGuppy::KingGuppy(BigGuppy *source, GameRegion *parent):
    Fish(source->centerPos(), new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 3, 10, 5), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(18, 26, 30, 30);

    //复制运动参数
    curV = source->curV;
    maxV = source->maxV;
    imgReverse = source->imgReverse;

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 3, 10, 5, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("smalleat.png"), 0, 3, 10, 5, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("smallturn.png"), 0, 3, 10, 5, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungryswim.png"), 0, 3, 10, 5, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungryeat.png"), 0, 3, 10, 5, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungryturn.png"), 0, 3, 10, 5, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("smalldie.png"), 0, 3, 10, 5, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void KingGuppy::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 600)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new Money(moneyID, centerPos(), gameRegion));
            health -= 150;
            timeToProduce = cdProduce;
        }
    }
}

Carnivore::Carnivore(const QPointF &pos, GameRegion *parent):
    Fish(pos, new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 4, 10, 5), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(4, 22, 48, 40);
    setZValue(4);

    //初始化参数
    maxV = b / (health + a) + c;
    curV.setX(-maxV);
    curV.setY(0);

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 4, 10, 5, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("smalleat.png"), 0, 4, 10, 5, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("smallturn.png"), 0, 4, 10, 5, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungryswim.png"), 0, 4, 10, 5, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungryeat.png"), 0, 4, 10, 5, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungryturn.png"), 0, 4, 10, 5, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("smalldie.png"), 0, 4, 10, 5, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("VoreEat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("VoreEat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void Carnivore::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态

        //更新运动参数
        {
            //确定最大速度
            maxV = b / (health + a) + c;

            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

            //确定捕食对象
            SmallGuppy* food = nullptr;
            QVector2D newV(curV.x() + random.bounded(-maxA, maxA), curV.y() + random.bounded(-maxA, 0.9 * maxA));
            if(state == HungrySwim || state == HungryReverse || timeToEat == 0)
            {
                food = static_cast<SmallGuppy*>(nearest(this, gameRegion->fishes, "SmallGuppy"));
                if(food)
                {
                    if(food->eated)
                    {
                        food = nullptr;
                    }
                    else
                    {
                        newV.setX(food->centerPos().x() - centerPos().x());
                        newV.setY(food->centerPos().y() - centerPos().y());
                        newV = newV.normalized() * maxV;
                    }
                }
            }

            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
            {
                newV.setX(-newV.x());
            }
            if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
            {
                newV.setY(curV.y() + 5 * maxA);
            }
            if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
            {
                newV.setY(curV.y() - 5 * maxA);
            }

            //设置掉头动画
            if(curV.x() * newV.x() < 0)
            {
                if(state == Swim || state == Eat)
                {
                    setState(Reverse);
                }
                if(state == HungrySwim || state == HungryEat)
                {
                    setState(HungryReverse);
                }
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

            //更新食物
            if(food && collidesWithItem(food))
            {
                if(state == Swim)
                    setState(Eat, food);
                if(state == HungrySwim)
                    setState(HungryEat, food);
            }
        }

        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 800)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new Money(moneyID, centerPos(), gameRegion));
            age -= 200;
            timeToProduce = cdProduce;
        }
    }
}

void Carnivore::toEat(Object *_food)
{
    SmallGuppy* food = static_cast<SmallGuppy*> (_food);
    state = Eat;
    food->eated = true;
    health += food->health;
    timeToEat = cdEat;
    frameAnimation* ani = aniFrames[Eat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(Swim);
    });
    setDefaultAnimation(ani);
    food->fade(true, 250);
}

void Carnivore::toHungryEat(Object *_food)
{
    SmallGuppy* food = static_cast<SmallGuppy*> (_food);
    state = HungryEat;
    food->eated = true;
    health += food->health;
    timeToEat = cdEat;
    frameAnimation* ani = aniFrames[HungryEat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(HungrySwim);
    });
    setDefaultAnimation(ani);
    food->fade(true, 250);
}

UltraVore::UltraVore(const QPointF &pos, GameRegion *parent):
    Fish(pos, new frameAnimation(gResourceManager->loadImage("ultravore.png"), 0, 0, 10, 4), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(10, 28, 80, 90);
    setZValue(4);

    //初始化参数
    maxV = b / (health + a) + c;
    curV.setX(-maxV);
    curV.setY(0);

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("ultravore.png"), 0, 0, 10, 4, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("ultravore.png"), 0, 1, 10, 4, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("ultravore.png"), 0, 2, 10, 4, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("ultravore.png"), 0, 3, 10, 4, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("VoreEat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::Die, aniDie);
}

void UltraVore::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态

        //更新运动参数
        {
            //确定最大速度
            maxV = b / (health + a) + c;

            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

            //确定捕食对象
            Carnivore* food = nullptr;
            QVector2D newV(curV.x() + random.bounded(-maxA, maxA), curV.y() + random.bounded(-maxA, 0.9 * maxA));
            if(state == HungrySwim || state == HungryReverse || timeToEat == 0)
            {
                food = static_cast<Carnivore*>(nearest(this, gameRegion->fishes, "Carnivore"));
                if(food)
                {
                    if(food->eated)
                    {
                        food = nullptr;
                    }
                    else
                    {
                        newV.setX(food->centerPos().x() - centerPos().x());
                        newV.setY(food->centerPos().y() - centerPos().y());
                        newV = newV.normalized() * maxV;
                    }
                }
            }

            //设定运动边框
            if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV.x() < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV.x() > 0))
            {
                newV.setX(-newV.x());
            }
            if(centerPos().y() < shape().boundingRect().height() / 2 &&  newV.y() < 0)
            {
                newV.setY(curV.y() + 5 * maxA);
            }
            if(centerPos().y() > gameRegion->boundingRect().height() - shape().boundingRect().height() / 2  && newV.y() > 0)
            {
                newV.setY(curV.y() - 5 * maxA);
            }

            //设置掉头动画
            if(curV.x() * newV.x() < 0)
            {
                if(state == Swim || state == Eat)
                {
                    setState(Reverse);
                }
                if(state == HungrySwim || state == HungryEat)
                {
                    setState(HungryReverse);
                }
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

            //更新食物
            if(food && collidesWithItem(food))
            {
                if(state == Swim)
                    setState(Eat, food);
                if(state == HungrySwim)
                    setState(HungryEat, food);
            }
        }

        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 800)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new Money(moneyID, centerPos(), gameRegion));
            health -= 250;
            timeToProduce = cdProduce;
        }
    }
}

void UltraVore::setState(Fish::State newState, Object *food)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Swim:
        if(aniDefault->state() == QTimeLine::NotRunning || state == HungrySwim)
        {
            toSwim();
        }
        else
        {
            connect(aniDefault, &frameAnimation::finished, [=](){
                toSwim();
            });
        }
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

void UltraVore::toEat(Object *_food)
{
    Carnivore* food = static_cast<Carnivore*> (_food);
    state = Eat;
    food->eated = true;
    health += food->health;
    timeToEat = cdEat;
    frameAnimation* ani = aniFrames[Eat];
    connect(ani, &frameAnimation::finished, [=](){
        setState(Swim);
    });
    setDefaultAnimation(ani);
    food->fade(true, 250);
}

Breeder::Breeder(const QPointF &pos, GameRegion *parent):
    Fish(pos, new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 3, 10, 9), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(11, 11, 55, 54);

    //初始化参数
    maxV = b / (health + a) + c;
    curV.setX(-maxV);
    curV.setY(0);

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 3, 10, 9, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 5, 10, 9, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 4, 10, 9, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 3, 10, 12, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 10, 10, 12, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 4, 10, 12, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 5, 10, 12, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void Breeder::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && health >= 600)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new SmallGuppy(centerPos(), gameRegion));
            gResourceManager->loadSound("Breed.wav")->play();
            health -= 100;
            timeToProduce = cdProduce;
        }

        //实现成长过程
        if(state == Swim && age >= 1000 && health >= 1000)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new BigBreeder(this, gameRegion));
            gResourceManager->loadSound("Grow.wav")->play();
            deleteLater();
        }
    }
}

BigBreeder::BigBreeder(Breeder* source, GameRegion *parent):
    Fish(source->centerPos(), new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 6, 10, 9), parent)
{
    timeToProduce = cdProduce;

    //初始化边框
    pathShape.addEllipse(4, 3, 63, 67);

    //复制运动参数
    curV = source->curV;
    maxV = source->maxV;
    imgReverse = source->imgReverse;

    //初始化动画
    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 6, 10, 9, this);
    frameAnimation* aniEat = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 8, 10, 9, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("breeder.png"), 0, 7, 10, 9, this);
    frameAnimation* aniHungrySwim = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 6, 10, 12, this);
    frameAnimation* aniHungryEat = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 11, 10, 12, this);
    frameAnimation* aniHungryReverse = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 7, 10, 12, this);
    frameAnimation* aniDie = new frameAnimation(gResourceManager->loadImage("hungrybreeder.png"), 0, 8, 10, 12, this);
    aniEat->setLoopCount(1);
    aniReverse->setLoopCount(1);
    aniHungryEat->setLoopCount(1);
    aniHungryReverse->setLoopCount(1);
    aniDie->setLoopCount(1);
    aniEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniHungryEat->setSound(gResourceManager->loadSound("Eat.wav"));
    aniDie->setSound(gResourceManager->loadSound("Die.wav"));
    setAnimations(Fish::Swim, aniSwim);
    setAnimations(Fish::Eat, aniEat);
    setAnimations(Fish::Reverse, aniReverse);
    setAnimations(Fish::HungrySwim, aniHungrySwim);
    setAnimations(Fish::HungryEat, aniHungryEat);
    setAnimations(Fish::HungryReverse, aniHungryReverse);
    setAnimations(Fish::Die, aniDie);
}

void BigBreeder::refresh()
{
    if(state == Die)
    {
        //死亡状态
        updateDieModel();
    }
    else
    {
        //存活状态
        updateMoveModel(); //更新运动参数
        updateStateModel(); //更新生理参数与状态

        //实现产出过程
        if(timeToProduce > 0)
        {
            timeToProduce--;
        }
        if(timeToProduce == 0 && age >= 1000 && health >= 1000)
        {
            GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
            gameRegion->addToGroup(new SmallGuppy(centerPos(), gameRegion));
            gResourceManager->loadSound("Breed.wav")->play();
            age -= cdProduce;
            timeToProduce = cdProduce;
        }
    }
}
