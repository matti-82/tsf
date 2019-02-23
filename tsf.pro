TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
    tsf.cpp \
    example.cpp

include(deployment.pri)
qtcAddDeployment()

HEADERS += \
    tsf.h

QMAKE_CXXFLAGS += -std=c++11
