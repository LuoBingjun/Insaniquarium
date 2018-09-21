#include "Scene.h"
#include "GameScene.h"

Scene::Scene(QObject *parent): QGraphicsScene(parent)
{
    setSceneRect(QRect(3, 1, 640, 480));

    //初始化背景音乐播放器
    bgmPlayer = new QMediaPlayer(this);
    bgmPlayer->setVolume(gSettings->value("General/volume").toInt());
    bgmPlayer->setPlaylist(new QMediaPlaylist(bgmPlayer));
    bgmPlayer->playlist()->setPlaybackMode(QMediaPlaylist::Loop);

    widgets = new QGraphicsItemGroup;
    widgets->setHandlesChildEvents(false);
    addItem(widgets);

    diaOptions = addWidget(new DiaOptions);
    diaOptions->setZValue(21);
    diaOptions->setVisible(false);

    QRectF pos(diaOptions->boundingRect());
    pos.moveCenter(QPointF(width() / 2, height() / 2));
    diaOptions->setPos(pos.topLeft());

    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::Ok, [=](){
        static_cast<MainWindow*>(gMainView->window())->loadSettings();
    });
}

Scene::~Scene()
{
    destroyItemGroup(widgets);
}

void Scene::setImgBg(QImage *bg)
{
    imgBg = bg;
}

void Scene::switchScene(Scene *newScene, const QVariant &infm)
{
    gMainView->setScene(newScene);
    deleteLater();
}

void Scene::showOptions()
{
    diaOptions->show();
}

void Scene::drawBackground(QPainter *painter, const QRectF &rect)
{
    painter->drawImage(rect, *imgBg, rect);
}

TitleScene::TitleScene(QObject *parent): Scene(parent)
{
    //设置背景图片
    setImgBg(gResourceManager->loadImage("titlescreen.jpg"));

    //设置背景音乐
    bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/Start.mp3"));
    bgmPlayer->play();

    //添加开始按钮
    Button* btnStart = new Button(QPoint(179, 415), gResourceManager->loadImage("loaderbar.png"), widgets);
    btnStart->setImgOver(gResourceManager->loadImage("loaderbarover.png"));
    btnStart->setImgPressed(gResourceManager->loadImage("loaderbarpressed.png"));
    btnStart->setVisible(false);
    connect(btnStart, &Button::clicked, this, &TitleScene::toSelectScene);
    widgets->addToGroup(btnStart);

    //添加进度条
    LoadingBar* barLoading = new LoadingBar(gResourceManager->loadImage("loaderbarloading.png"), widgets);
    connect(barLoading, &LoadingBar::finished, [=](){
        barLoading->setVisible(false);
        btnStart->setVisible(true);
    });
    barLoading->start();
    widgets->addToGroup(barLoading);
}

void TitleScene::toSelectScene()
{
    Scene* newScene = new SelectScene;
    newScene->bgmPlayer->deleteLater();
    newScene->bgmPlayer = bgmPlayer;
    newScene->bgmPlayer->setParent(newScene);
    switchScene(newScene);
}

SelectScene::SelectScene(QObject *parent): Scene(parent)
{
    //设置背景图片
    setImgBg(gResourceManager->loadImage("selectorback.png"));

    //设置背景音乐
    bgmPlayer->playlist()->addMedia(QUrl::fromLocalFile("music/Start.mp3"));
    bgmPlayer->play();

    //添加尾巴动画
    AniItem* aniTail = new AniItem(gResourceManager->loadImage("tail.png"), 0, 0, 8, 1, this);
    aniTail->setPos(38, 305);
    aniTail->setLoopCount(1);
    QTimer* tmrAni = new QTimer(this);
    tmrAni->setInterval(5000);
    connect(tmrAni, &QTimer::timeout, aniTail, &AniItem::start);
    tmrAni->start();
    widgets->addToGroup(aniTail);

    //添加文本框
    QGraphicsTextItem *item = new QGraphicsTextItem;
    item->setPos(90, 65);
    item->setFont(QFont("Blambot Pro", 14));
    item->setDefaultTextColor(QColor(25, 26, 119));
    item->setPlainText(gSettings->value("User/name").toString());
    item->setTextWidth(160);
    QTextBlockFormat format;
    format.setAlignment(Qt::AlignCenter);
    QTextCursor cursor = item->textCursor();
    cursor.select(QTextCursor::Document);
    cursor.mergeBlockFormat(format);
    cursor.clearSelection();
    item->setTextCursor(cursor);
    addItem(item);

    //添加按钮
    Button* btnStartAdventure = new Button(QPoint(357, 48), gResourceManager->loadImage("StartAdventureNormal.png"), widgets);
    btnStartAdventure->setImgOver(gResourceManager->loadImage("StartAdventureOver.png"));
    btnStartAdventure->setImgPressed(gResourceManager->loadImage("StartAdventurePressed.png"));
    connect(btnStartAdventure, &Button::clicked, [&](){
        gMainView->toGameScene();
        deleteLater();
    });
    widgets->addToGroup(btnStartAdventure);

    Button* btnTimeTrial = new Button(QPoint(358, 142), gResourceManager->loadImage("TimeTrialNormal.png"), widgets);
    btnTimeTrial->setImgOver(gResourceManager->loadImage("TimeTrialOver.png"));
    btnTimeTrial->setImgPressed(gResourceManager->loadImage("TimeTrialPressed.png"));
    widgets->addToGroup(btnTimeTrial);

    Button* btnChallenge = new Button(QPoint(358, 212), gResourceManager->loadImage("ChallengeNormal.png"), widgets);
    btnChallenge->setImgOver(gResourceManager->loadImage("ChallengeOver.png"));
    btnChallenge->setImgPressed(gResourceManager->loadImage("ChallengePressed.png"));
    widgets->addToGroup(btnChallenge);

    Button* btnVirtualTank = new Button(QPoint(357, 287), gResourceManager->loadImage("VirtualTankNormal.png"), widgets);
    btnVirtualTank->setImgOver(gResourceManager->loadImage("VirtualTankOver.png"));
    btnVirtualTank->setImgPressed(gResourceManager->loadImage("VirtualTankPressed.png"));
    widgets->addToGroup(btnVirtualTank);

    Button* btnOptions = new Button(QPoint(326, 413), gResourceManager->loadImage("btnOptions.png"), widgets);
    btnOptions->setImgOver(gResourceManager->loadImage("btnOptionsOver.png"));
    btnOptions->setImgPressed(gResourceManager->loadImage("btnOptionsPressed.png"));
    widgets->addToGroup(btnOptions);
    connect(btnOptions, &Button::clicked, this, &SelectScene::showOptions);

    Button* btnHelp = new Button(QPoint(421, 413), gResourceManager->loadImage("btnHelp.png"), widgets);
    btnHelp->setImgOver(gResourceManager->loadImage("btnHelpOver.png"));
    btnHelp->setImgPressed(gResourceManager->loadImage("btnHelpPressed.png"));
    widgets->addToGroup(btnHelp);

    Button* btnQuit = new Button(QPoint(515, 413), gResourceManager->loadImage("btnQuit.png"), widgets);
    btnQuit->setImgOver(gResourceManager->loadImage("btnQuitOver.png"));
    btnQuit->setImgPressed(gResourceManager->loadImage("btnQuitPressed.png"));
    widgets->addToGroup(btnQuit);
    connect(btnQuit, &Button::clicked, [=](){
        gMainView->window()->close();
    });

    Button* btnHallofFame = new Button(QPoint(401, 381), gResourceManager->loadImage("btnHallofFame.png"), widgets);
    btnHallofFame->setImgOver(gResourceManager->loadImage("btnHallofFameOver.png"));
    btnHallofFame->setImgPressed(gResourceManager->loadImage("btnHallofFamePressed.png"));
    widgets->addToGroup(btnHallofFame);
}

HatchScene::HatchScene(int _id, QObject *parent): Scene(parent), id(_id)
{
    //初始化DiaOptions
    connect(static_cast<DiaOptions*>(diaOptions->widget()), &DiaOptions::BacktoMenu, [=](){
        switchScene(new SelectScene);
    });

    //初始化Button
    Button* btnMenu = new Button(QPointF(525, 4), gResourceManager->loadImage("btnMenuNormal.png"));
    addItem(btnMenu);
    btnMenu->setImgOver(gResourceManager->loadImage("btnMenuOver.png"));
    btnMenu->setImgPressed(gResourceManager->loadImage("btnMenuPressed.png"));
    connect(btnMenu, &Button::clicked, this, &HatchScene::showOptions);

    Button* btnContinue = new Button(QPointF(186, 446), gResourceManager->loadImage("btnContinueNormal.png"));
    addItem(btnContinue);
    btnContinue->setImgOver(gResourceManager->loadImage("btnContinueOver.png"));
    btnContinue->setImgPressed(gResourceManager->loadImage("btnContinuePressed.png"));

    if(id < 5)
    {
        switch(id)
        {
        case 1:
            setImgBg(gResourceManager->loadImage("hatchscene1.png"));
            aniShow = new AniItem(gResourceManager->loadImage("scl_stinky.png"), 0, 0, 10, 1, this);
            break;
        case 2:
            setImgBg(gResourceManager->loadImage("hatchscene2.png"));
            aniShow = new AniItem(gResourceManager->loadImage("scl_niko.png"), 0, 0, 10, 1, this);
            break;
        case 3:
            setImgBg(gResourceManager->loadImage("hatchscene3.png"));
            aniShow = new AniItem(gResourceManager->loadImage("scl_itchy.png"), 0, 0, 10, 1, this);
            break;
        case 4:
            setImgBg(gResourceManager->loadImage("hatchscene4.png"));
            aniShow = new AniItem(gResourceManager->loadImage("scl_vert.png"), 0, 0, 10, 1, this);
            break;
        }
        addItem(aniShow);
        aniShow->setPos(290, 100);
        aniShow->start();

        connect(btnContinue, &Button::clicked, [=](){
            gMainView->toGameScene();
        });
    }
    else
    {
        gMainView->setGameScene(nullptr);
        setImgBg(gResourceManager->loadImage("hatchscene5.png"));
        connect(btnContinue, &Button::clicked, [=](){
            switchScene(new SelectScene);
        });
    }

    //设置背景音乐
    bgmPlayer->setMedia(QUrl::fromLocalFile("music/Win.mp3"));
    bgmPlayer->play();
}
