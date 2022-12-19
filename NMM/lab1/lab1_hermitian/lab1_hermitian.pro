TEMPLATE = app
CONFIG += console c++17
CONFIG -= app_bundle
CONFIG -= qt

SOURCES += \
        main.cpp \
        spline.cpp

HEADERS += \
    gnuplot-iostream.h \
    spline.h

INCLUDEPATH += /home/andrew/Ucheba/cpp_libs/eigen/

LIBS += -lboost_iostreams -lboost_system -lboost_filesystem
