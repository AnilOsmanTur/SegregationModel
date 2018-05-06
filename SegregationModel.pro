TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += main.cpp \
    segmodel.cpp

INCLUDEPATH += /usr/local/opencv2/

LIBS += -lopencv_core -lopencv_highgui

HEADERS += \
    segmodel.h
