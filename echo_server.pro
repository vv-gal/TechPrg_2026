QT -= gui
QT += network #для работы с сетью

CONFIG += c++11 console
CONFIG -= app_bundle

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    main.cpp \
    echotcpserver.cpp \      #
    mathsolver.cpp

HEADERS += \
    echotcpserver.h \        #
    mathsolver.h
