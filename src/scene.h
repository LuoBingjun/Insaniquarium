#pragma once

#include <QGraphicsScene>
#include <QPainter>
#include <QMediaPlayer>
#include <QMediaPlaylist>
#include "MainWindow.h"
#include "DiaOptions.h"
#include "Widget.h"

//场景基类
class Scene : public QGraphicsScene
{
    Q_OBJECT

public:
    explicit Scene(QObject *parent = 0);
    virtual ~Scene();
    void setImgBg(QImage* bg);
    virtual void switchScene(Scene* newScene, const QVariant& infm = 0);
    virtual void showOptions();

    QMediaPlayer* bgmPlayer = nullptr;

protected:
    QGraphicsItemGroup* widgets;
    QGraphicsProxyWidget* diaOptions;
    virtual void drawBackground(QPainter *painter, const QRectF &rect);

private:
    QImage *imgBg = nullptr;
};

class TitleScene : public Scene
{
    Q_OBJECT

public:
    explicit TitleScene(QObject *parent = 0);
    void toSelectScene();

};

class SelectScene : public Scene
{
    Q_OBJECT

public:
    explicit SelectScene(QObject *parent = 0);
};

class HatchScene : public Scene
{
    Q_OBJECT

public:
    explicit HatchScene(int _id, QObject *parent = 0);

protected:
    int id;
    AniItem* aniShow;
};

