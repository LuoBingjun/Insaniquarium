#include "Pet.h"
#include "Money.h"
#include "Alien.h"

const double maxA = 0.05;

Pet::Pet(const QPointF &pos, frameAnimation *ani, GameRegion *parent):
    Object(pos, ani, parent)
{
    if(parent)
    {
        parent->pets.append(this);
    }
    setZValue(2);
}

Pet::~Pet()
{
    if(parentItem())
    {
        static_cast<GameRegion*>(parentItem())->pets.removeAll(this);
    }
}

QPainterPath Pet::shape() const
{
    return pathShape;
}

int Pet::type() const
{
    return pet;
}

Pet *Pet::fromData(const QVariant &source, GameRegion *parent)
{
    dataPet data = source.value<dataPet>();
    Pet* rt;
    if(data.type == "Stinky")
    {
        rt = new Stinky(data.pos, parent);
    }
    else if(data.type == "Niko")
    {
        rt = new Niko(data.pos, parent);
    }
    else if(data.type == "Itchy")
    {
        rt = new Itchy(data.pos, parent);
    }
    else if(data.type == "Vert")
    {
        rt = new Vert(data.pos, parent);
    }

    rt->curV = data.curV;
    rt->curVf = data.curVf;

    rt->alive = data.alive;
    rt->imgReverse = data.imgReverse;
    rt->Opacity = data.Opacity;

    return rt;
}

QVariant Pet::toData()
{
    dataPet data;
    Pet* rt = this;
    data.curV = rt->curV;
    data.curVf = rt->curVf;

    data.alive = rt->alive;
    data.imgReverse = rt->imgReverse;
    data.Opacity = rt->Opacity;
    data.pos = pos();

    data.type = metaObject()->className();

    QVariant a;
    a.setValue(data);

    return a;
}

Stinky::Stinky(const QPointF &pos, GameRegion *parent) : random(QTime::currentTime().msec()),
    Pet(pos, new frameAnimation(gResourceManager->loadImage("stinky.png"), 0, 0, 10, 3), parent)
{
    pathShape.addRect(3, 16, 76, 48);

    //初始化帧动画
    frameAnimation* aniCrawl = new frameAnimation(gResourceManager->loadImage("stinky.png"), 0, 0, 10, 3, this);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("stinky.png"), 0, 1, 10, 3, this);
    frameAnimation* aniRetracted = new frameAnimation(gResourceManager->loadImage("stinky.png"), 0, 2, 10, 3, this);
    aniReverse->setLoopCount(1);
    aniRetracted->setLoopCount(1);
    aniFrames[Crawl] = aniCrawl;
    aniFrames[Reverse] = aniReverse;
    aniFrames[Retracted] = aniRetracted;

    setReverse();
}

void Stinky::setState(Stinky::State newState)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Crawl:
        toCrawl();
        break;

    case Reverse:
        toReverse();
        break;

    case Retracted:
        toRetracted();
        break;
    }
}

void Stinky::toCrawl()
{
    state = Crawl;
    frameAnimation* ani = aniFrames[Crawl];
    setDefaultAnimation(ani);
}

void Stinky::toReverse()
{
    state = Reverse;
    frameAnimation* ani = aniFrames[Reverse];
    connect(ani, &frameAnimation::finished, [=](){
        setReverse();
        if(imgReverse)
        {
            curVf = qAbs(curVf);
        }
        else
        {
            curVf = -qAbs(curVf);
        }
        setState(Crawl);
    });
    setDefaultAnimation(ani);
}

void Stinky::toRetracted()
{
    state = Retracted;
    frameAnimation* ani = aniFrames[Retracted];
    setDefaultAnimation(ani);
}

void Stinky::refresh()
{
    if(static_cast<GameScene*>(scene())->normalState)
    {
        GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

        double newV = curVf + random.bounded(-maxA, maxA);

        //寻找食物
        Money* food = nearest(this, gameRegion->moneys);
        if(food)
        {
            if(food->centerPos().x() > centerPos().x())
                newV = maxV;
            else
                newV = -maxV;
        }

        //设定运动边框
        if((centerPos().x() < shape().boundingRect().width() / 2 &&  newV < 0) || (centerPos().x() > gameRegion->boundingRect().width() - shape().boundingRect().width() / 2 && newV > 0))
        {
            newV = -newV;
        }

        //设置掉头动画
        if(curVf * newV < 0)
        {
            setState(Reverse);
        }

        //防止无限旋转
        if(food && qAbs(food->centerPos().x() - centerPos().x()) < 1)
            newV = 0;

        //速度限制
        if(qAbs(newV) > maxV)
        {
            if(newV > 0)
                newV = maxV;
            else
                newV = -maxV;
        }

        //更新速度和位移
        curVf = newV;
        if(state != Reverse)
        {
            moveBy(curVf, 0);
            setState(Crawl);
        }

        //收获Money
        for(auto& iter : collidingItems())
        {
            if(iter->type() == money)
            {
                static_cast<GameScene*>(scene())->money += static_cast<Money*>(iter)->value;
                gResourceManager->loadSound("Getmoney.wav")->play();
                static_cast<Money*>(iter)->deleteLater();
            }
        }
    }
    else
    {
        setState(Retracted);
    }
}

Niko::Niko(const QPointF &pos, GameRegion *parent):
    Pet(pos, new frameAnimation(gResourceManager->loadImage("niko.png"), 0, 0, 10, 3), parent)
{
    frameAnimation* aniOff = new frameAnimation(gResourceManager->loadImage("niko.png"), 0, 0, 10, 3, this);
    frameAnimation* aniOpen = new frameAnimation(gResourceManager->loadImage("niko.png"), 0, 1, 10, 3, this);
    frameAnimation* aniClose = new frameAnimation(gResourceManager->loadImage("niko.png"), 0, 2, 10, 3, this);
    aniOpen->setLoopCount(1);
    aniClose->setLoopCount(1);
    aniClose->setFrameRange(9, 0);

    aniFrames[Off] = aniOff;
    aniFrames[Open] = aniOpen;
    aniFrames[Close] = aniClose;
}

void Niko::refresh()
{
    timer--;
    if(timer == 0)
    {
        timer = cdTime;
        if(state == Off)
            open();
        else if(state == On)
            close();
    }
}

void Niko::open()
{
    state = On;
    gResourceManager->loadSound("nikoOpen.wav")->play();

    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());
    m_Pearl = new Pearl(mapToParent(40, 42), gameRegion, this);
    gameRegion->addToGroup(m_Pearl);
    m_Pearl->setVisible(false);

    setDefaultAnimation(aniFrames[Open]);

    connect(aniDefault, &frameAnimation::finished, [=](){
        m_Pearl->setVisible(true);
    });
}

void Niko::close()
{
    state = Off;
    gResourceManager->loadSound("nikoClose.wav")->play();

    if(m_Pearl)
    {
        m_Pearl->deleteLater();
    }

    setDefaultAnimation(aniFrames[Close]);
    connect(aniDefault, &frameAnimation::finished, [=](){
        setDefaultAnimation(aniFrames[Off]);
    });
}

void Niko::picked()
{
    m_Pearl = nullptr;
    setDefaultAnimation(aniFrames[Close]);
    aniDefault->stop();
}

Itchy::Itchy(const QPointF &pos, GameRegion *parent): random(QTime::currentTime().msec()),
    Pet(pos, new frameAnimation(gResourceManager->loadImage("itchy.png"), 0, 0, 10, 4), parent)
{
    pathShape.addEllipse(22, 23, 33, 33);

    curV.setX(-maxV);
    curV.setY(0);

    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("itchy.png"), 0, 0, 10, 4, parent);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("itchy.png"), 0, 1, 10, 4, parent);
    aniReverse->setLoopCount(1);
    aniFrames[Swim] = aniSwim;
    aniFrames[Reverse] = aniReverse;
}

void Itchy::refresh()
{
    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    //确定追击对象
    Alien* food = nullptr;
    QVector2D newV(curV.x() + random.bounded(-maxA, maxA), curV.y() + random.bounded(-maxA, 0.9 * maxA));

    if(gameRegion->aliens.size())
    {
        food = nearest(this, gameRegion->aliens);
        if(food)
        {
            newV.setX(food->centerPos().x() - centerPos().x());
            newV.setY(food->centerPos().y() - centerPos().y());
            newV = newV.normalized() * maxV;
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
    else if(food && collidesWithItem(food))
    {
        food->health -= 10;
    }
    else
    {
        moveBy(curV.x(), curV.y());
    }
}

void Itchy::setState(Itchy::State newState)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Swim:
        if(aniDefault->state() == QTimeLine::NotRunning)
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

    case Reverse:
        toReverse();
        break;
    }
}

void Itchy::toSwim()
{
    state = Swim;
    frameAnimation* ani = aniFrames[Swim];
    setDefaultAnimation(ani);
}

void Itchy::toReverse()
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

Vert::Vert(const QPointF &pos, GameRegion *parent) :
    Pet(pos, new frameAnimation(gResourceManager->loadImage("vert.png"), 0, 0, 10, 2), parent)
{
    curV.setX(-maxV);
    curV.setY(0);

    frameAnimation* aniSwim = new frameAnimation(gResourceManager->loadImage("vert.png"), 0, 0, 10, 2, parent);
    frameAnimation* aniReverse = new frameAnimation(gResourceManager->loadImage("vert.png"), 0, 1, 10, 2, parent);
    aniReverse->setLoopCount(1);
    aniFrames[Swim] = aniSwim;
    aniFrames[Reverse] = aniReverse;
}

void Vert::refresh()
{
    GameRegion* gameRegion = static_cast<GameRegion*>(parentItem());

    //产生Money
    if(timer > 0)
        timer--;
    if(timer == 0)
    {
        gameRegion->addToGroup(new Money(1, centerPos(), gameRegion));
        timer = cdTime;
    }

    QVector2D newV(curV.x() + random.bounded(-maxA, maxA), curV.y() + random.bounded(-maxA, 0.9 * maxA));

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

void Vert::setState(Vert::State newState)
{
    if(newState == state)
        return;

    switch(newState)
    {
    case Swim:
        if(aniDefault->state() == QTimeLine::NotRunning)
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

    case Reverse:
        toReverse();
        break;
    }
}

void Vert::toSwim()
{
    state = Swim;
    frameAnimation* ani = aniFrames[Swim];
    setDefaultAnimation(ani);
}

void Vert::toReverse()
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
