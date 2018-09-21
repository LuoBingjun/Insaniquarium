#include "utils.h"
#include "GameScene.h"
#include "Fish.h"
#include "Food.h"
#include "Money.h"
#include "Alien.h"
#include "Pet.h"

ResourceManager::ResourceManager()
{
    images.insert("NULL", new QImage);
    sounds.insert("NULL", new QSoundEffect);
}

ResourceManager::~ResourceManager()
{
    for(auto& iter : images)
    {
        delete iter;
    }

    for(auto& iter : sounds)
    {
        delete iter;
    }
}

QImage* ResourceManager::loadImage(const QString &filename)
{
    if(!images.contains(filename))
        images.insert(filename, new QImage(":/res/images/" + filename));

    return images[filename];
}

QSoundEffect *ResourceManager::loadSound(const QString &filename)
{
    if(!sounds.contains(filename))
    {
        QSoundEffect* sound = new QSoundEffect;
        sound->setSource(QUrl::fromLocalFile("sound/" + filename));
        sound->setLoopCount(1);
        sound->setVolume(1.0);
        sounds.insert(filename, sound);
    }
    return sounds[filename];
}


frameAnimation::frameAnimation(QImage *img, int x_num, int y_num, int xFrames, int yFrames, QObject *parent):
    QTimeLine(400, parent), imgFrame(img)
{
    setLoopCount(0);
    setFrameRange(0, xFrames - 1);
    frameWidth = img->width() / xFrames;
    frameHeight = img->height() / yFrames;
    beginX = frameWidth * x_num;
    beginY = frameHeight * y_num;

    rect.setTopLeft(QPointF(0, 0));
    rect.setSize(QSizeF(frameWidth, frameHeight));

}

QRectF frameAnimation::getFrame() const
{
    return QRectF(beginX + frameWidth * currentFrame(), beginY, frameWidth, frameHeight);
}

QRectF frameAnimation::getRect() const
{
    return rect;
}

QImage* frameAnimation::getImg() const
{
    return imgFrame;
}

void frameAnimation::setPos(const QPointF& pos)
{
    rect.moveTopLeft(pos);
}

void frameAnimation::setSound(QSoundEffect *_sound)
{
    sound = _sound;
    connect(this, &frameAnimation::stateChanged, [=](QTimeLine::State newState){
        if(newState == QTimeLine::Running)
        {
            sound->play();
        }
    });
}


GameRegion::GameRegion(const QPointF &pos, const QSizeF size, GameScene *parent) :
    QObject(parent), rectBounding(QPointF(0, 0), size), scene(parent)
{
    setPos(pos);

    //初始化边界
    top.moveTo(0, 0);
    top.lineTo(size.width(), 0);
    bottom.moveTo(0, size.height());
    bottom.lineTo(size.width(), size.height());
    left.moveTo(0, 0);
    left.lineTo(0, size.height());
    right.moveTo(size.width(), 0);
    right.lineTo(size.width(), size.height());
}

GameRegion::GameRegion(const dataGameRegion &source, const QPointF &pos, const QSizeF size, GameScene *parent):
    QObject(parent), rectBounding(QPointF(0, 0), size), scene(parent)
{
    setPos(pos);

    //初始化边界
    top.moveTo(0, 0);
    top.lineTo(size.width(), 0);
    bottom.moveTo(0, size.height());
    bottom.lineTo(size.width(), size.height());
    left.moveTo(0, 0);
    left.lineTo(0, size.height());
    right.moveTo(size.width(), 0);
    right.lineTo(size.width(), size.height());

    battle = source.battle;


    for(auto& iter : source.fishes)
    {
        addToGroup(Fish::fromData(iter, this));
    }

    for(auto& iter : source.foods)
    {
        addToGroup(Food::fromData(iter, this));
    }

    for(auto& iter : source.moneys)
    {
        addToGroup(Money::fromData(iter, this));
    }

    for(auto& iter : source.aliens)
    {
        addToGroup(Alien::fromData(iter, this));
    }

    for(auto& iter : source.pets)
    {
        addToGroup(Pet::fromData(iter, this));
    }
}

QRectF GameRegion::boundingRect() const
{
    return rectBounding;
}

dataGameRegion GameRegion::toData()
{
    dataGameRegion data;
    data.battle = battle;
    for(auto& iter : fishes)
    {
        data.fishes.append(iter->toData());
    }

    for(auto& iter : foods)
    {
        data.foods.append(iter->toData());
    }

    for(auto& iter : aliens)
    {
        data.aliens.append(iter->toData());
    }

    for(auto& iter : pets)
    {
        data.pets.append(iter->toData());
    }

    for(auto& iter : moneys)
    {
        if(QString(iter->metaObject()->className()) == "Pearl")
            continue;
        data.moneys.append(iter->toData());
    }
}

bool GameRegion::sceneEventFilter(QGraphicsItem *watched, QEvent *event)
{
    if(!scene->running)
        return true;

    if(event->type() == QEvent::GraphicsSceneMousePress)
    {
        QGraphicsSceneMouseEvent *mouseEvent = static_cast<QGraphicsSceneMouseEvent*>(event);
        if(mouseEvent->button() == Qt::LeftButton)
        {
            Object* obj = static_cast<Object*>(watched);
            if(obj->type() == Object::money)
            {
                obj->onleftMousePress(mouseEvent);
                return true;
            }

            if(!battle)
            {
                return true;
            }
            else
            {
                if(QString(obj->metaObject()->className()) == "Sylv" || QString(obj->metaObject()->className()) == "Balrog")
                {
                    obj->onleftMousePress(mouseEvent);
                }
                return false;
            }
        }
    }
    return true;
}

void GameRegion::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if(!scene->running)
        return;

    if(!battle)
    {
        if(foods.size() < scene->foodnum && scene->money >= 5)
        {
            scene->money -= 5;
            gResourceManager->loadSound("Dropfood.wav")->play();
            addToGroup(new Food(scene->foodtype, event->pos(), this));
        }
    }
    else
    {
        gResourceManager->loadSound("Hit.wav")->play();
    }
}
