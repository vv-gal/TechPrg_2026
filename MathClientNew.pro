QT += core widgets network printsupport

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

SOURCES += \
    main.cpp \
    authview.cpp \
    registrationview.cpp \
    mainview.cpp \
    functionmodel.cpp \
    servercontroller.cpp \
    qcustomplot.cpp          # ← ДОБАВЬТЕ

HEADERS += \
    authview.h \
    registrationview.h \
    mainview.h \
    functionmodel.h \
    servercontroller.h \
    qcustomplot.h            # ← ДОБАВЬТЕ

TARGET = MathClient
TEMPLATE = app

DEFINES += QT_DEPRECATED_WARNINGS
