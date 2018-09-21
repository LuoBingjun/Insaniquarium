#-------------------------------------------------
#
# Project created by QtCreator 2018-09-01T16:47:12
#
#-------------------------------------------------

QT       += core gui multimedia

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Insaniquarium
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    Scene.cpp \
    Widget.cpp \
    MainWindow.cpp \
    Object.cpp \
    Fish.cpp \
    GameScene.cpp \
    Food.cpp \
    utils.cpp \
    Money.cpp \
    Alien.cpp \
    Pet.cpp \
    DiaOptions.cpp \
    DiaFail.cpp

HEADERS += \
    Scene.h \
    Widget.h \
    MainWindow.h \
    Object.h \
    Alien.h \
    Pet.h \
    Fish.h \
    GameScene.h \
    Food.h \
    utils.h \
    Money.h \
    DiaOptions.h \
    DiaFail.h

FORMS += \
    DiaOptions.ui \
    DiaFail.ui

RESOURCES += \
    resources.qrc
