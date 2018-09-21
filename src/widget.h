//控件类
#pragma once

#include <QGraphicsView>
#include <QGraphicsWidget>
#include <QGraphicsSimpleTextItem>
#include <QGraphicsProxyWidget>
#include <QTimeLine>
#include <QGraphicsSceneMouseEvent>
#include <QVariantAnimation>
#include <QSoundEffect>
#include "MainWindow.h"
#include "utils.h"

class Stinky;
class Object;
class GameScene;


class Button : public QGraphicsObject
{
    Q_OBJECT

public:
    explicit Button(const QPointF& pos, QImage *img, QGraphicsItem *parent = 0);
    explicit Button(const QPointF& pos, QImage *img, const QSize& size, QGraphicsItem *parent = 0);
    virtual QRectF boundingRect() const;
    void setImgOver(QImage *img);
    void setImgPressed(QImage *img);

signals:
    void clicked();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

    bool playSound = true;

protected:
    enum State {Normal, Over, Pressed};
    State state = Normal;
    QImage* imgNormal;
    QImage* imgOver;
    QImage* imgPressed;
    QRectF rectBounding;
};

class LoadingBar : public QVariantAnimation, public QGraphicsItem
{
    Q_OBJECT

public:
    explicit LoadingBar(QImage *img, QGraphicsItemGroup* parent = 0);
    virtual QRectF boundingRect() const;

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    int currentWidth = 0;
    QRectF rectBounding;
    QImage* imgBar;
    Stinky* stinky;
};

class AniItem : public frameAnimation, public QGraphicsItem
{
    Q_OBJECT

public:
    AniItem(QImage* img, int x_num, int y_num, int xFrames, int yFrames, QObject *parent = 0);
    using QGraphicsItem::setPos;
    QRectF boundingRect() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
};

class Scene;
class MenuButton;

class MenuBar : public QGraphicsObject
{
public:
    MenuBar(GameScene* parent = 0);
    void addMenuButton(MenuButton* menuButton);
    void refresh();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QRectF boundingRect() const;

private:
    QImage* imgBar = nullptr;
    MenuButton* mButtons[7] = { nullptr };
    Button* btnMenu = nullptr;
    QGraphicsTextItem* txtMoney = nullptr;
};

struct dataMenuBar
{
    QString type[7];
};

Q_DECLARE_METATYPE(dataMenuBar)

class MenuButton : public Button
{
    Q_OBJECT

public:
    MenuButton(int _order, int _price, MenuBar* parent = 0);

    int price;
    friend class MenuBar;

public slots:
    void enable();
    virtual void toBuy() = 0;

protected:
    const static int posX[7];
    const int order;
    QPointF posFront;
    QImage* imgFront = nullptr;
    frameAnimation* aniChanged;
    frameAnimation* aniShow = nullptr;
    QGraphicsSimpleTextItem* txtPrice;

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);
    virtual QPainterPath shape() const;
};

class MbtnGuppy : public MenuButton
{
    Q_OBJECT

public:
    MbtnGuppy(int order, MenuBar* parent = 0);
    virtual void toBuy();
};

class MbtnFoodType : public MenuButton
{
    Q_OBJECT

public:
    MbtnFoodType(int order, MenuBar* parent = 0);
    virtual void toBuy();

protected:
    QImage* imgFronts[2] = { nullptr };
};

class MbtnFoodNum : public MenuButton
{
    Q_OBJECT

public:
    MbtnFoodNum(int order, MenuBar* parent = 0);
    virtual void toBuy();

protected:
    QGraphicsSimpleTextItem* txtNum;
};

class MbtnCarnivore : public MenuButton
{
    Q_OBJECT

public:
    MbtnCarnivore(int order, MenuBar* parent = 0);
    virtual void toBuy();
};

class MbtnUltraVore : public MenuButton
{
    Q_OBJECT

public:
    MbtnUltraVore(int order, MenuBar* parent = 0);
    virtual void toBuy();
};

class MbtnBreeder : public MenuButton
{
    Q_OBJECT

public:
    MbtnBreeder(int order, MenuBar* parent = 0);
    virtual void toBuy();
};

class MbtnEgg : public MenuButton
{
    Q_OBJECT

public:
    MbtnEgg(int order, int price, MenuBar* parent = 0);
    virtual void toBuy();

protected:
    QImage* imgFronts[3] = { nullptr };
};

class MbtnWeapon : public MenuButton
{
    Q_OBJECT

public:
    MbtnWeapon(int order, MenuBar* parent = 0);
    virtual void toBuy();

private:
    int weaponid;
};
