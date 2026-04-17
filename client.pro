QT += core gui network widgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += warn_off  # Отключаем предупреждения о deprecated

TARGET = client
TEMPLATE = app

SOURCES += \
    main.cpp \
    controller/clientcontroller.cpp \
    network/serverconnector.cpp \
    view/loginwindow.cpp \
    view/registerwindow.cpp \
    view/mainwindow.cpp \
    view/graphpage.cpp

HEADERS += \
    controller/clientcontroller.h \
    network/serverconnector.h \
    view/loginwindow.h \
    view/registerwindow.h \
    view/mainwindow.h \
    view/graphpage.h

RESOURCES += \
    resources.qrc

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
