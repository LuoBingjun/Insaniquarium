#include "MainWindow.h"
#include "DiaOptions.h"
#include "Fish.h"
#include "Food.h"
#include "Alien.h"
#include "Money.h"
#include "Pet.h"

#include <QApplication>

QSettings* gSettings;
MainView* gMainView;
ResourceManager* gResourceManager;

int main(int argc, char *argv[])
{
    QCoreApplication::setOrganizationName("Luo Bingjun");
    QCoreApplication::setOrganizationDomain("lbjthu.xin");
    QCoreApplication::setApplicationName("Insaniquarium");

    QApplication a(argc, argv);

    //初始化全局变量
    gSettings = new QSettings("settings.ini", QSettings::IniFormat);
    gResourceManager = new ResourceManager;

    //初始化全局字体
    QFontDatabase::addApplicationFont(":/res/font/Junglefever.ttf");
    QFontDatabase::addApplicationFont(":/res/font/BlambotPro.ttf");
    QFontDatabase::addApplicationFont(":/res/font/ContinuumBold.ttf");
    QFontDatabase::addApplicationFont(":/res/font/Liddie.ttf");
    QFontDatabase::addApplicationFont(":/res/font/Pixetl.ttf");

    //注册类型
    qRegisterMetaType<dataFish>("dataFish");
    qRegisterMetaType<dataFood>("dataFood");
    qRegisterMetaType<dataAlien>("dataAlien");
    qRegisterMetaType<dataMoney>("dataMoney");
    qRegisterMetaType<dataPet>("dataPet");
    qRegisterMetaType<dataGameRegion>("dataGameRegion");
    qRegisterMetaType<dataGameScene>("dataGameScene");

    MainWindow w;
    QIcon icon(":/res/images/icon.ico");
    w.setWindowIcon(icon);
    w.setWindowTitle("Insaniquarium Delexue");
    w.show();

    return a.exec();
}
