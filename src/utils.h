#pragma once

#include <QtGui>
#include <QRandomGenerator>
#include <QTimeLine>
#include <QGraphicsItemGroup>
#include <QSoundEffect>

class Fish;
class Food;
class Money;
class Alien;
class Pet;
class GameScene;

//资源管理器类
class ResourceManager
{
public:
    ResourceManager();
    ~ResourceManager();

    QImage* loadImage(const QString& filename);
    QSoundEffect* loadSound(const QString &filename);

private:
    QMap<QString, QImage*> images;
    QMap<QString, QSoundEffect*> sounds;
};

//随机数生成器类
class randomGenerator : public QRandomGenerator
{
public:
    using QRandomGenerator::QRandomGenerator;
    using QRandomGenerator::bounded;
    double bounded(double min, double max){
        return bounded(max - min) + min;
    }
};

//帧动画类
class frameAnimation : public QTimeLine
{
    Q_OBJECT

public:
    frameAnimation(QImage* img, int x_num, int y_num, int xFrames, int yFrames, QObject *parent = 0);
    QRectF getFrame() const;
    QRectF getRect() const;
    QImage* getImg() const;
    void setPos(const QPointF& pos);
    void setSound(QSoundEffect* _sound);

private:
    QSoundEffect* sound = nullptr;
    int beginX;
    int beginY;
    int frameWidth;
    int frameHeight;
    QRectF rect;
    QImage* imgFrame;
};

//游戏有效区域类
struct dataGameRegion
{
    QList<QVariant> fishes;
    QList<QVariant> foods;
    QList<QVariant> moneys;
    QList<QVariant> aliens;
    QList<QVariant> pets;
    bool battle;
};

Q_DECLARE_METATYPE(dataGameRegion)

class GameRegion : public QObject, public QGraphicsItemGroup
{
    Q_OBJECT

public:
    QLinkedList<Fish*> fishes;
    QLinkedList<Food*> foods;
    QLinkedList<Money*> moneys;
    QLinkedList<Alien*> aliens;
    QLinkedList<Pet*> pets;

    QPainterPath top;
    QPainterPath bottom;
    QPainterPath left;
    QPainterPath right;

    bool battle = false;


    GameRegion(const QPointF &pos, const QSizeF size, GameScene *parent = 0);
    GameRegion(const dataGameRegion& source, const QPointF &pos, const QSizeF size, GameScene *parent = 0);
    virtual QRectF boundingRect() const;
    dataGameRegion toData();


protected:
    virtual bool sceneEventFilter(QGraphicsItem *watched, QEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);

private:
    QRectF rectBounding;
    GameScene* scene;
};
