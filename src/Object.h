#pragma once

#include <QGraphicsItemGroup>
#include <QGraphicsObject>
#include <QTimeLine>
#include "MainWindow.h"
#include "GameScene.h"

//游戏物体基类
class Object : public QGraphicsObject
{
    Q_OBJECT

public:
    enum {fish = UserType + 1, food = UserType + 2, alien = UserType + 3, money = UserType + 4, pet = UserType + 5};

    Object(const QPointF& pos, frameAnimation *ani, GameRegion* parent = 0);
    virtual ~Object();
    virtual QVariant toData() = 0;
    virtual void onleftMousePress(QGraphicsSceneMouseEvent* event); //响应鼠标点击事件
    virtual void refresh() = 0; //刷新函数接口
    void setDefaultAnimation(frameAnimation* ani); //设置默认动画
    void setReverse(); //图片翻转
    QPointF centerPos(); //中心坐标

    virtual QRectF boundingRect() const;
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

    bool alive = true;

public slots:
    void fade(bool living = false, int duration = 1000);

protected:
    frameAnimation* aniDefault = nullptr; //默认动画
    QRectF rectBounding;
    QTransform transReverse;
    bool imgReverse = false; //图片是否翻转
    double Opacity = 1; //透明度
};

//寻找最近Object的模板函数
template <typename T>
inline T* nearest(Object* object1, const QLinkedList<T*>& objects, const QString& classname = 0)
{
    T* nearest = nullptr;
    int shortest = INT_MAX;
    QLineF length;
    length.setP1(object1->centerPos());
    if(classname == 0)
    {
        for(auto& object2 : objects)
        {
            if(object2->alive)
            {
                length.setP2(object2->centerPos());
                if(length.length() < shortest)
                {
                    shortest = length.length();
                    nearest = object2;
                }
            }
        }
    }
    else
    {
        for(auto& object2 : objects)
        {
            if(object2->alive && QString(object2->metaObject()->className()) == classname)
            {
                length.setP2(object2->centerPos());
                if(length.length() < shortest)
                {
                    shortest = length.length();
                    nearest = object2;
                }
            }
        }
    }
    return nearest;
}
