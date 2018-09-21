#include "GameScene.h"
#include "Fish.h"
#include "Food.h"
#include "Alien.h"
#include "Pet.h"
#include "Money.h"

const QString imgBg;

GameScene::GameScene(int _id, QObject *parent): Scene(parent), id(_id)
{
    //开挂键
    connect(gMainView->btnToAdd, &QAbstractButton::clicked, [&](){
        money += 500;
    });

    connect(gMainView->btnToWin, &QAbstractButton::clicked, [&](){
        win();
    });

    //初始化关卡提示
    QGraphicsSimpleTextItem* txtTank = new QGraphicsSimpleTextItem;
    addItem(txtTank);
    txtTank->setBrush(QBrush(QColor(255, 255, 255, 150)));
    txtTank->setPos(14, 445);
    txtTank->setText("Tank " + QString::number(id));

    QFont font("Continuum Bold");
    font.setPixelSize(18);
    txtTank->setFont(font);

    //初始化背景音乐
    alienPlayer = new QMediaPlayer(this);
    alienPlayer->setVolume(gSettings->value("General/volume").toInt());

    //初始化Menu界面
    connect(diaOptions, &QGraphicsProxyWidget::visibleChanged, [=](){
        if(diaOptions->isVisible())
            running = false;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::Ok, [=](){
        running = true;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::Back, [=](){
        running = true;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::BacktoMenu, [=](){
        pause();
        gMainView->setScene(new SelectScene);
    });

    //初始化Fail界面
    DiaFail* fail = new DiaFail;
    diaFail = addWidget(fail);
    diaFail->setZValue(11);
    diaFail->setVisible(false);

    QRectF pos(diaFail->boundingRect());
    pos.moveCenter(QPointF(width() / 2, height() / 2));
    diaFail->setPos(pos.topLeft());

    connect(fail, &DiaFail::clicked, [=](){
        gMainView->setGameScene(new GameScene(id, gMainView));
        gMainView->toGameScene();
    });

    //初始化游戏有效区域
    gameRegion = new GameRegion(QPointF(31, 97), QSizeF(580, 320), this);
    addItem(gameRegion);

    //初始化MenuBar
    menuBar = new MenuBar(this);
    addItem(menuBar);

    //初始化游戏计时器
    mainTimer = new QTimer(this);
    mainTimer->setInterval(20);
    connect(mainTimer, &QTimer::timeout, this, &GameScene::refresh);

    init(id);
}

QVariant GameScene::toData()
{
    dataGameScene source;
    source.id = id;
    source.money = money;
    source.time = time;
    source.weapon = weapon;
    source.foodtype = foodtype;
    source.foodnum = foodnum;
    source.gamepoint = gamepoint;
    source.normalState = normalState;
    source.running = running;
    source.gameRegion = gameRegion->toData();

    QVariant a;
    a.setValue(source);
    return a;
}

GameScene::GameScene(const dataGameScene &source, QObject *parent): Scene(parent)
{
    id = source.id;
    money = source.money;
    time = source.time;
    weapon = source.weapon;
    foodtype = source.foodtype;
    foodnum = source.foodnum;
    gamepoint = source.gamepoint;
    normalState = source.normalState;
    running = source.running;

    gameRegion = new GameRegion(source.gameRegion, QPointF(31, 97), QSizeF(580, 320), this);

    alienPlayer->setMedia(QUrl::fromLocalFile("music/Alien2.mp3"));

    //开挂键
    connect(gMainView->btnToAdd, &QAbstractButton::clicked, [&](){
        money += 500;
    });

    connect(gMainView->btnToWin, &QAbstractButton::clicked, [&](){
        win();
    });

    //初始化关卡提示
    QGraphicsSimpleTextItem* txtTank = new QGraphicsSimpleTextItem;
    addItem(txtTank);
    txtTank->setBrush(QBrush(QColor(255, 255, 255, 150)));
    txtTank->setPos(14, 445);
    txtTank->setText("Tank " + QString::number(id));

    QFont font("Continuum Bold");
    font.setPixelSize(18);
    txtTank->setFont(font);

    //初始化背景音乐
    alienPlayer = new QMediaPlayer(this);
    alienPlayer->setVolume(gSettings->value("General/volume").toInt());

    //初始化Menu界面
    connect(diaOptions, &QGraphicsProxyWidget::visibleChanged, [=](){
        if(diaOptions->isVisible())
            running = false;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::Ok, [=](){
        running = true;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::Back, [=](){
        running = true;
    });
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::BacktoMenu, [=](){
        pause();
        gMainView->setScene(new SelectScene);
    });

    //初始化Fail界面
    DiaFail* fail = new DiaFail;
    diaFail = addWidget(fail);
    diaFail->setZValue(11);
    diaFail->setVisible(false);

    QRectF pos(diaFail->boundingRect());
    pos.moveCenter(QPointF(width() / 2, height() / 2));
    diaFail->setPos(pos.topLeft());

    connect(fail, &DiaFail::clicked, [=](){
        gMainView->setGameScene(new GameScene(id, gMainView));
        gMainView->toGameScene();
    });

    //初始化游戏有效区域
    gameRegion = new GameRegion(source.gameRegion, QPointF(31, 97), QSizeF(580, 320), this);
    addItem(gameRegion);

    //初始化MenuBar
    menuBar = new MenuBar(this);
    addItem(menuBar);

    //初始化游戏计时器
    mainTimer = new QTimer(this);
    mainTimer->setInterval(20);
    connect(mainTimer, &QTimer::timeout, this, &GameScene::refresh);

    //初始化背景音乐和MenuBar
    switch(id)
    {
    case 1:
        setImgBg(gResourceManager->loadImage("aquarium1.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene1.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 150, menuBar));
        break;
    case 2:
        setImgBg(gResourceManager->loadImage("aquarium1.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene1.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 500, menuBar));
        break;

    case 3:
        setImgBg(gResourceManager->loadImage("aquarium2.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene2.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 1000, menuBar));
        break;

    case 4:
        setImgBg(gResourceManager->loadImage("aquarium3.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene3.mp3"));

        menuBar->addMenuButton(new MbtnBreeder(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 3000, menuBar));
        break;

    case 5:
        setImgBg(gResourceManager->loadImage("aquarium4.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene4.mp3"));

        menuBar->addMenuButton(new MbtnBreeder(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnUltraVore(4, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 5000, menuBar));
        break;
    }
}

GameScene::~GameScene()
{
    destroyItemGroup(gameRegion);
}

bool GameScene::tryBuying(int price)
{
    if(money >= price)
    {
        money -= price;
        gResourceManager->loadSound("Buy.wav")->play();
        return true;
    }
    else
    {
        gResourceManager->loadSound("Buzzer.wav")->play();
        return false;
    }
}

void GameScene::toAlienState(int time)
{
    switch(id)
    {
    case 1:
        return;

    case 2:
    case 3:
    case 4:
        if(time % 30000 != 0)
            return;
        break;

    case 5:
        if(time % 50000 != 0)
            return;
        break;
    }

    //添加敌人提示
    QGraphicsSimpleTextItem* txtEnemy = new QGraphicsSimpleTextItem;
    addItem(txtEnemy);
    txtEnemy->setBrush(QBrush(QColor(255, 0, 0)));
    txtEnemy->setPos(223, 451);
    txtEnemy->setText("ENEMY APPROACHING");

    QFont font("Continuum Bold");
    font.setPixelSize(20);
    txtEnemy->setFont(font);


    gResourceManager->loadSound("alienAppear.wav")->play();
    QVariantAnimation* aniAppear = new QVariantAnimation(this);
    aniAppear->setStartValue(bgmPlayer->volume());
    aniAppear->setEndValue(0);
    aniAppear->setDuration(4000);
    alienPlayer->setMedia(QUrl::fromLocalFile("music/Alien1.mp3"));

    connect(aniAppear, &QVariantAnimation::valueChanged, [=](const QVariant& value){
        bgmPlayer->setVolume(value.toInt());
    });
    connect(aniAppear, &QVariantAnimation::finished, [=](){
        bgmPlayer->pause();
        aniAppear->deleteLater();
        alienPlayer->play();
    });

    aniAppear->start();

    switch(id)
    {
    case 2:
        QTimer::singleShot(5000, [=](){
            alienPlayer->stop();
            gResourceManager->loadSound("Roar.wav")->play();
            gameRegion->addToGroup(new Sylv(QPointF(200, 200), gameRegion));
            gameRegion->battle = true;
            removeItem(txtEnemy);
        });
        break;

    case 3:
        QTimer::singleShot(5000, [=](){
            alienPlayer->stop();
            gResourceManager->loadSound("Roar.wav")->play();
            gameRegion->addToGroup(new Balrog(QPointF(200, 200), gameRegion));
            gameRegion->battle = true;
            removeItem(txtEnemy);
        });
        break;

    case 4:
        QTimer::singleShot(5000, [=](){
            alienPlayer->stop();
            gResourceManager->loadSound("Gus.wav")->play();
            gameRegion->addToGroup(new Gus(QPointF(200, 200), gameRegion));
            removeItem(txtEnemy);
        });
        break;

    case 5:
        QTimer::singleShot(5000, [=](){
            alienPlayer->stop();
            gResourceManager->loadSound("Roar.wav")->play();
            gameRegion->addToGroup(new Sylv(QPointF(250, 160), gameRegion));
            gameRegion->addToGroup(new Balrog(QPointF(500, 240), gameRegion));
            gameRegion->battle = true;
            removeItem(txtEnemy);
        });
        break;
    }
}

void GameScene::setState(bool newState)
{
    if(newState == normalState)
        return;

    normalState = newState;

    if(newState)
    {
        gameRegion->battle = false;
        QVariantAnimation* aniAppear = new QVariantAnimation(this);
        aniAppear->setStartValue(0);
        aniAppear->setEndValue(gSettings->value("General/volume").toInt());
        aniAppear->setDuration(2000);
        connect(aniAppear, &QVariantAnimation::valueChanged, [=](const QVariant& value){
            bgmPlayer->setVolume(value.toInt());
        });
        connect(aniAppear, &QVariantAnimation::finished, [=](){
            aniAppear->deleteLater();
        });
        aniAppear->start();
        alienPlayer->stop();
        bgmPlayer->play();
    }
    else
    {
        alienPlayer->setMedia(QUrl::fromLocalFile("music/Alien2.mp3"));
        alienPlayer->play();
    }
}

void GameScene::refresh()
{
    if(!running)
        return;

    //判断胜负
    if(gameRegion->fishes.size() == 0)
    {
        fail();
    }

    if(gamepoint == 3)
    {
        win();
    }

    //计时
    time += mainTimer->interval();

    toAlienState(time);

    //刷新GameScene状态
    if(gameRegion->aliens.size())
    {
        setState(false);
    }
    else
    {
        setState(true);
    }

    //刷新Object状态
    for(auto& iter : gameRegion->childItems())
    {
        Object* ob = static_cast<Object*>(iter);
        if(ob->alive)
        {
            ob->refresh();
        }
    }

    //刷新menuBar状态
    menuBar->refresh();
}

void GameScene::pause()
{
    running = false;
    if(normalState)
    {
        bgmPlayer->pause();
    }
    else
    {
        alienPlayer->pause();
    }
}

void GameScene::start()
{
    mainTimer->start();
    running = true;
    if(normalState)
    {
        bgmPlayer->play();
    }
    else
    {
        alienPlayer->play();
    }
}

void GameScene::init(int id)
{
    switch(id)
    {
    case 1:
        setImgBg(gResourceManager->loadImage("aquarium1.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene1.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 150, menuBar));

        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 0), gameRegion));
        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 200), gameRegion));
        break;
    case 2:
        setImgBg(gResourceManager->loadImage("aquarium1.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene1.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 500, menuBar));

        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 0), gameRegion));
        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 200), gameRegion));
        gameRegion->addToGroup(new Stinky(QPointF(200, 320), gameRegion));
        break;

    case 3:
        setImgBg(gResourceManager->loadImage("aquarium2.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene2.mp3"));

        menuBar->addMenuButton(new MbtnGuppy(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 1000, menuBar));

        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 0), gameRegion));
        gameRegion->addToGroup(new SmallGuppy(QPointF(100, 200), gameRegion));
        gameRegion->addToGroup(new Stinky(QPointF(200, 320), gameRegion));
        gameRegion->addToGroup(new Niko(gameRegion->mapFromScene(215, 205), gameRegion));
        break;

    case 4:
        setImgBg(gResourceManager->loadImage("aquarium3.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene3.mp3"));

        menuBar->addMenuButton(new MbtnBreeder(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 3000, menuBar));

        gameRegion->addToGroup(new Breeder(QPointF(100, 0), gameRegion));
        gameRegion->addToGroup(new Stinky(QPointF(200, 320), gameRegion));
        gameRegion->addToGroup(new Niko(gameRegion->mapFromScene(195, 305), gameRegion));
        gameRegion->addToGroup(new Itchy(QPointF(300, 0), gameRegion));
        break;

    case 5:
        setImgBg(gResourceManager->loadImage("aquarium4.jpg"));
        bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/GameScene4.mp3"));

        menuBar->addMenuButton(new MbtnBreeder(0, menuBar));
        menuBar->addMenuButton(new MbtnFoodType(1, menuBar));
        menuBar->addMenuButton(new MbtnFoodNum(2, menuBar));
        menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));
        menuBar->addMenuButton(new MbtnUltraVore(4, menuBar));
        menuBar->addMenuButton(new MbtnWeapon(5, menuBar));
        menuBar->addMenuButton(new MbtnEgg(6, 5000, menuBar));

        gameRegion->addToGroup(new Breeder(QPointF(100, 0), gameRegion));
        gameRegion->addToGroup(new Breeder(QPointF(100, 200), gameRegion));
        gameRegion->addToGroup(new Stinky(QPointF(200, 320), gameRegion));
        gameRegion->addToGroup(new Niko(gameRegion->mapFromScene(105, 225), gameRegion));
        gameRegion->addToGroup(new Itchy(QPointF(300, 0), gameRegion));
        gameRegion->addToGroup(new Vert(QPointF(300, 100), gameRegion));
        break;
    }

    /*
    menuBar->addMenuButton(new MbtnCarnivore(3, menuBar));


    //添加鱼
    gameRegion->addToGroup(new Niko(gameRegion->mapFromScene(135, 295), gameRegion));
    gameRegion->addToGroup(new Itchy(QPointF(300, 0), gameRegion));
    gameRegion->addToGroup(new Vert(QPointF(100, 100), gameRegion));
    //gameRegion->addToGroup(new Breeder(QPointF(200, 0), gameRegion));
    //gameRegion->addToGroup(new Carnivore(QPointF(100, 100), gameRegion));
    //gameRegion->addToGroup(new UltraVore(QPointF(200, 200), gameRegion));
    //gameRegion->addToGroup(new Sylv(QPointF(200, 200), gameRegion));
    //gameRegion->addToGroup(new Stinky(QPointF(100, 320), gameRegion));
    //gameRegion->addToGroup(new Money(0, QPointF(100, 100), gameRegion));
*/
}

void GameScene::win()
{
    gMainView->btnToAdd->disconnect();
    gMainView->btnToWin->disconnect();
    gMainView->setGameScene(new GameScene(id + 1));
    switchScene(new HatchScene(id));
}

void GameScene::fail()
{
    running = false;
    mainTimer->stop();
    diaFail->show();
    gMainView->btnToAdd->disconnect();
    gMainView->btnToWin->disconnect();
}

