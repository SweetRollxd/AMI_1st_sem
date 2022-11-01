TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp

HEADERS += \
    gnuplot-iostream.h

LIBS += -lboost_iostreams -lboost_system -lboost_filesystem
