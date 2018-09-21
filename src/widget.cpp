#include "Widget.h"
#include "Food.h"
#include "Fish.h"
#include "Pet.h"
#include "GameScene.h"
#include "utils.h"

Button::Button(const QPointF& pos, QImage *img, QGraphicsItem *parent) :
    imgNormal(img), imgOver(img), imgPressed(img), QGraphicsObject(parent),
    rectBounding(img->rect())
{
    setPos(pos);
    setAcceptHoverEvents(true);
}

Button::Button(const QPointF& pos, QImage *img, const QSize &size, QGraphicsItem *parent) :
    imgNormal(img), imgOver(img), imgPressed(img), QGraphicsObject(parent),
    rectBounding(QPointF(0, 0), size)
{
    setPos(pos);
    setAcceptHoverEvents(true);
}

QRectF Button::boundingRect() const
{
    return rectBounding;
}

void Button::setImgOver(QImage *img)
{
    imgOver = img;
}

void Button::setImgPressed(QImage *img)
{
    imgPressed = img;
}

void Button::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(state == State::Normal)
    {
        painter->drawImage(imgNormal->rect(), *imgNormal);
    }
    else if(state == State::Over)
    {
        painter->drawImage(imgOver->rect(), *imgOver);
    }
    else if(state == State::Pressed)
    {
        painter->drawImage(imgPressed->rect(), *imgPressed);
    }
}

void Button::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    state = State::Over;
    update();
}

void Button::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    state = State::Normal;
    update();
}

void Button::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    state = State::Pressed;
    update();
    if(playSound)
    {
        gResourceManager->loadSound("buttonClick.wav")->play();
    }
}

void Button::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    if(contains(event->pos()))
    {
        emit clicked();
        state = State::Over;
    }
    else
    {
        state = State::Normal;
    }
    update();
}

LoadingBar::LoadingBar(QImage *img, QGraphicsItemGroup *parent):
    QGraphicsItem(parent), imgBar(img), rectBounding(img->rect())
{
    setPos(QPointF(179, 415));

    stinky = new Stinky(QPointF(160, 422));
    scene()->addItem(stinky);

    setStartValue(0);
    setEndValue(imgBar->width());
    setEasingCurve(QEasingCurve::InQuad);
    setDuration(3000);

    connect(this, &QVariantAnimation::valueChanged, [=](const QVariant &value){
        currentWidth = value.toInt();
        stinky->setPos(160 + currentWidth, stinky->y());
        update();
    });
    connect(this, &LoadingBar::finished, [=](){
        stinky->fade();
    });
}

QRectF LoadingBar::boundingRect() const
{
    return rectBounding;
}

void LoadingBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    QRectF rectDraw(0, 0, currentWidth, imgBar->height());
    painter->drawImage(rectDraw, *imgBar, rectDraw);
}

MenuBar::MenuBar(GameScene *parent)
{
    setParent(parent);
    setPos(0, 0);

    imgBar = gResourceManager->loadImage("menubar.png");

    //初始化txtMoney
    txtMoney = new QGraphicsTextItem(this);
    txtMoney->setPos(550, 34);
    txtMoney->setTextWidth(80);
    txtMoney->setDefaultTextColor(QColor(170, 246, 85));

    QFont font("Continuum Bold");
    font.setPixelSize(18);
    txtMoney->setFont(font);

    //初始化btnMenu
    QImage* imgBtnOver = gResourceManager->loadImage("OPTIONSBUTTON.png");
    QImage* imgBtnPressed = gResourceManager->loadImage("OPTIONSBUTTONd.png");
    btnMenu = new Button(QPointF(525, 3), gResourceManager->loadImage("NULL"), imgBtnOver->size(), this);
    btnMenu->setImgOver(imgBtnOver);
    btnMenu->setImgPressed(imgBtnPressed);

    connect(btnMenu, &Button::clicked, [=](){
        static_cast<GameScene*>(scene())->showOptions();
    });
}

void MenuBar::addMenuButton(MenuButton *menuButton)
{
    mButtons[menuButton->order] = menuButton;
    menuButton->setParentItem(this);
}

void MenuBar::refresh()
{
    GameScene* gameScene = static_cast<GameScene*>(scene());
    txtMoney->setPlainText(QString::number(gameScene->money));
    for(auto iter : mButtons)
    {
        if(iter && gameScene->money > iter->price)
        {
            iter->enable();
        }
    }
}

void MenuBar::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(imgBar->rect(), *imgBar);
}

QRectF MenuBar::boundingRect() const
{
    return imgBar->rect();
}

const int MenuButton::posX[7] = {19, 88, 145, 218, 291, 364, 437};

MenuButton::MenuButton(int _order, int _price, MenuBar *parent):
    Button(QPointF(posX[_order], 3), gResourceManager->loadImage("mbuttonu.png"), parent), order(_order), price(_price)
{
    setParentItem(parent);
    setVisible(false);

    setImgOver(gResourceManager->loadImage("mbuttono.png"));
    setImgPressed(gResourceManager->loadImage("mbuttond.png"));
    playSound = false;

    posFront.setX(0);
    posFront.setY(0);

    //初始化开头动画
    aniChanged = new frameAnimation(gResourceManager->loadImage("HatchANIMATION.png"), 0, 0, 3, 1, this);
    aniChanged->setDuration(50);
    aniChanged->setLoopCount(1);
    connect(aniChanged, &frameAnimation::frameChanged, [=](){
        update();
    });
    connect(aniChanged, &frameAnimation::finished, [&](){
        aniChanged->deleteLater();
        aniChanged = nullptr;
    });

    //处理点击事件
    connect(this, &MbtnGuppy::clicked, [=](){
        if(static_cast<GameScene*>(scene())->normalState
                && price > 0 && static_cast<GameScene*>(scene())->tryBuying(price))
            toBuy();
    });

    //初始化txtPrice
    txtPrice = new QGraphicsSimpleTextItem(this);
    txtPrice->setPen(QPen(QColor(109, 249, 109)));
    txtPrice->setPos(14, 45);
    txtPrice->setText("$" + QString::number(price));

    QFont font("PixetlRegular");
    font.setPixelSize(12);
    txtPrice->setFont(font);
}

void MenuButton::enable()
{
    if(!isVisible())
    {
        setVisible(true);
        aniChanged->start();
    }
}

void MenuButton::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(state == State::Normal)
    {
        painter->drawImage(imgNormal->rect(), *imgNormal);
    }
    else if(state == State::Over)
    {
        painter->drawImage(imgOver->rect(), *imgOver);
    }
    else if(state == State::Pressed)
    {
        painter->drawImage(imgPressed->rect(), *imgPressed);
    }

    if(imgFront)
    {
        painter->drawImage(QRectF(posFront, imgFront->size()), *imgFront, imgFront->rect());
    }

    if(aniShow)
    {
        painter->drawImage(aniShow->getRect(), *aniShow->getImg(), aniShow->getFrame());
    }

    if(aniChanged && aniChanged->state() == QTimeLine::Running)
    {
        painter->drawImage(aniChanged->getRect(), *aniChanged->getImg(), aniChanged->getFrame());
    }
}

QPainterPath MenuButton::shape() const
{
    QPainterPath path;
    path.addEllipse(5, 2, 47, 41);
    return path;
}

MbtnGuppy::MbtnGuppy(int order, MenuBar *parent): MenuButton(order, 100, parent)
{
    aniShow = new frameAnimation(gResourceManager->loadImage("smallswim.png"), 0, 0, 10, 5, this);
    aniShow->setPos(QPointF(-11.5, -18));

    connect(aniChanged, &frameAnimation::stateChanged, [=](QTimeLine::State newState){
        if(newState == QTimeLine::Running)
            aniShow->start();
    });
    connect(aniShow, &frameAnimation::frameChanged, [=](){
        update();
    });
}

void MbtnGuppy::toBuy()
{
    GameRegion* gameRegion = static_cast<GameScene*>(scene())->gameRegion;
    Fish* obj = new SmallGuppy(QPointF(500, 0), gameRegion);
    gameRegion->addToGroup(obj);
    obj->appear();
    gResourceManager->loadSound("SplashIntoWater.wav")->play();
}

MbtnFoodType::MbtnFoodType(int order, MenuBar *parent) :
    MenuButton(order, 200, parent)
{
    posFront.setX(9);
    posFront.setY(2);

    imgFronts[0] = gResourceManager->loadImage("food1.png");
    imgFronts[1] = gResourceManager->loadImage("food2.png");

    imgFront = imgFronts[static_cast<GameScene*>(scene())->foodtype];
}

void MbtnFoodType::toBuy()
{
    static_cast<GameScene*>(scene())->foodtype++;
    if(static_cast<GameScene*>(scene())->foodtype >= 2)
    {
        price = -1;
        imgFront = nullptr;
        txtPrice->setText(" MAX");
    }
    else
    {
        imgFront = imgFronts[static_cast<GameScene*>(scene())->foodtype];
    }
}

MbtnFoodNum::MbtnFoodNum(int order, MenuBar *parent):
    MenuButton(order, 300, parent)
{
    txtNum = new QGraphicsSimpleTextItem(this);
    txtNum->setBrush(QBrush(QColor(108, 247, 108)));
    txtNum->setPen(QPen(QColor(17, 12, 41)));
    txtNum->setPos(22, 10);
    txtNum->setText(QString::number(static_cast<GameScene*>(scene())->foodnum + 1));

    QFont font("Continuum Bold");
    font.setPixelSize(20);
    txtNum->setFont(font);
}

void MbtnFoodNum::toBuy()
{
    static_cast<GameScene*>(scene())->foodnum++;
    txtNum->setText(QString::number(static_cast<GameScene*>(scene())->foodnum + 1));
}

MbtnCarnivore::MbtnCarnivore(int order, MenuBar *parent) :
    MenuButton(order, 1000, parent)
{
    aniShow = new frameAnimation(gResourceManager->loadImage("scl_oscar.png"), 0, 0, 10, 1, this);
    aniShow->setPos(QPointF(8, 2));

    connect(aniChanged, &frameAnimation::stateChanged, [=](QTimeLine::State newState){
        if(newState == QTimeLine::Running)
            aniShow->start();
    });
    connect(aniShow, &frameAnimation::frameChanged, [=](){
        update();
    });
}

void MbtnCarnivore::toBuy()
{
    GameRegion* gameRegion = static_cast<GameScene*>(scene())->gameRegion;
    Fish* obj = new Carnivore(QPointF(500, 0), gameRegion);
    gameRegion->addToGroup(obj);
    obj->appear();
    gResourceManager->loadSound("SplashIntoWater.wav")->play();
}

MbtnBreeder::MbtnBreeder(int order, MenuBar *parent):
    MenuButton(order, 200, parent)
{
    aniShow = new frameAnimation(gResourceManager->loadImage("scl_breeder.png"), 0, 0, 10, 1, this);
    aniShow->setPos(QPointF(8, 2));

    connect(aniChanged, &frameAnimation::stateChanged, [=](QTimeLine::State newState){
        if(newState == QTimeLine::Running)
            aniShow->start();
    });
    connect(aniShow, &frameAnimation::frameChanged, [=](){
        update();
    });
}

void MbtnBreeder::toBuy()
{
    GameRegion* gameRegion = static_cast<GameScene*>(scene())->gameRegion;
    Fish* obj = new Breeder(QPointF(500, 0), gameRegion);
    gameRegion->addToGroup(obj);
    obj->appear();
    gResourceManager->loadSound("SplashIntoWater.wav")->play();
}

MbtnEgg::MbtnEgg(int order, int price, MenuBar *parent):
    MenuButton(order, price, parent)
{
    posFront.setX(6);
    posFront.setY(3);

    imgFronts[0] = gResourceManager->loadImage("EggPieces1.png");
    imgFronts[1] = gResourceManager->loadImage("EggPieces2.png");
    imgFronts[2] = gResourceManager->loadImage("EggPieces3.png");

    imgFront = imgFronts[static_cast<GameScene*>(scene())->gamepoint];
}

void MbtnEgg::toBuy()
{
    static_cast<GameScene*>(scene())->gamepoint++;
    if(static_cast<GameScene*>(scene())->gamepoint <= 2)
    {
        imgFront = imgFronts[static_cast<GameScene*>(scene())->gamepoint];
    }
}

AniItem::AniItem(QImage *img, int x_num, int y_num, int xFrames, int yFrames, QObject *parent):
    frameAnimation(img, x_num, y_num, xFrames, yFrames, parent)
{
    connect(this, &AniItem::frameChanged, [=](){
        update();
    });
}

QRectF AniItem::boundingRect() const
{
    return getRect();
}

void AniItem::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    painter->drawImage(getRect(), *getImg(), getFrame());
}

MbtnUltraVore::MbtnUltraVore(int order, MenuBar *parent):
    MenuButton(order, 10000, parent)
{
    aniShow = new frameAnimation(gResourceManager->loadImage("scl_ultra.png"), 0, 0, 10, 1, this);
    aniShow->setPos(QPointF(8, 2));

    connect(aniChanged, &frameAnimation::stateChanged, [=](QTimeLine::State newState){
        if(newState == QTimeLine::Running)
            aniShow->start();
    });
    connect(aniShow, &frameAnimation::frameChanged, [=](){
        update();
    });
}

void MbtnUltraVore::toBuy()
{
    GameRegion* gameRegion = static_cast<GameScene*>(scene())->gameRegion;
    Fish* obj = new UltraVore(QPointF(500, 0), gameRegion);
    gameRegion->addToGroup(obj);
    obj->appear();
    gResourceManager->loadSound("SplashIntoWater.wav")->play();
}

MbtnWeapon::MbtnWeapon(int order, MenuBar *parent):
    MenuButton(order, 1000, parent)
{
    aniShow = new frameAnimation(gResourceManager->loadImage("LazerGunz.png"), 0, 0, 10, 1, this);
    aniShow->setPos(QPointF(5, 5));
    weaponid = (static_cast<GameScene*>(scene())->weapon - 400) / 200;
    aniShow->setFrameRange(weaponid, weaponid);
}

void MbtnWeapon::toBuy()
{
    static_cast<GameScene*>(scene())->weapon += 200;
    weaponid++;
    if(weaponid == 9)
    {
        price = -1;
        txtPrice->setText(" MAX");
    }

    aniShow->setFrameRange(weaponid, weaponid);
}
