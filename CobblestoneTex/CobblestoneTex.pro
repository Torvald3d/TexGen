#-------------------------------------------------
#
# Project created by QtCreator 2013-03-10T16:59:24
#
#-------------------------------------------------

QT       += core gui\
            opengl

QMAKE_CXXFLAGS = -std=c++0x

TARGET = CobblestoneTex
TEMPLATE = app


SOURCES += main.cpp\
        widget.cpp \
    proceduraltexture.cpp \
    glwidget.cpp

HEADERS  += widget.h \
    proceduraltexture.h \
    glwidget.h

FORMS    += widget.ui

OTHER_FILES += \
    note.txt \
    shader.frag \
    shader.vert

RESOURCES += \
    shaders.qrc
