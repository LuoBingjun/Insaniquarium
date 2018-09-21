#include "MainWindow.h"
#include "GameScene.h"
#include "Scene.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    gMainView = new MainView(this);
    gMainView->setFrameShape(QFrame::NoFrame);
    gMainView->show();

    setStyleSheet("background-color:black;");
    setFixedSize(633, 468);
    loadSettings();
}

QRect MainWindow::getViewRect() const
{
    QSize ViewSize(640, 480);
    ViewSize.scale(size(), Qt::KeepAspectRatio);
    QRect ViewRect;
    ViewRect.setSize(ViewSize);
    ViewRect.moveCenter(QPoint(width() / 2, height() / 2));
    return ViewRect;
}

void MainWindow::loadSettings()
{
    if(gSettings->value("General/fullscreen").toBool())
        setWindowState(Qt::WindowFullScreen);
    else
        setWindowState(Qt::WindowNoState);
    if(gMainView->scene())
    {
        static_cast<Scene*>(gMainView->scene())->bgmPlayer->setVolume(gSettings->value("General/volume").toInt());
    }
}

MainWindow::~MainWindow()
{

}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    if(isFullScreen())
    {
        gMainView->setGeometry(getViewRect());
    }
    else
    {
        gMainView->setGeometry(QRect(0, -1, 640, 480));
    }
}

MainView::MainView(MainWindow *parent) : QGraphicsView(parent), mainwindow(parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setScene(new TitleScene);

    btnToWin = new QPushButton(this);
    btnToAdd = new QPushButton(this);
    btnToWin->setFixedSize(0, 0);
    btnToAdd->setFixedSize(0, 0);
    btnToWin->setFlat(true);
    btnToAdd->setFlat(true);
    btnToWin->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_W));
    btnToAdd->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_A));
}

MainWindow *MainView::getMainWindow() const
{
    return mainwindow;
}

void MainView::toGameScene()
{
    if(!curGame)
        curGame = new GameScene;

    if(scene())
        scene()->deleteLater();

    curGame->start();
    setScene(curGame);
}

void MainView::setGameScene(GameScene *newGame)
{
    if(curGame)
    {
        curGame->deleteLater();
    }
    curGame = newGame;
}

void MainView::resizeEvent(QResizeEvent *event)
{
    fitInView(sceneRect(), Qt::KeepAspectRatio);
}
