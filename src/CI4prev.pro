# -------------------------------------------------
# Project created by QtCreator 2009-11-14T18:04:42
# -------------------------------------------------
QT       += core gui sql network declarative printsupport
TARGET = CI4prev
TEMPLATE = app
unix:INCLUDEPATH += /usr/include/gsl
unix:LIBS += /usr/lib/libgsl.so \
    /usr/lib/libgslcblas.so
win32:LIBS += C:/GSL-1.15/lib/libgsl.dll.a \
    C:/GSL-1.15/lib/libgslcblas.dll.a
win32:INCLUDEPATH += C:/GSL-1.15/include
SOURCES += main.cpp \
    mainwindow.cpp \
    frmsettings.cpp \
    frmabout.cpp \
    frmhelp.cpp
HEADERS += mainwindow.h \
    frmsettings.h \
    frmabout.h \
    frmhelp.h
FORMS += mainwindow.ui \
    frmsettings.ui \
    frmabout.ui \
    frmhelp.ui
RESOURCES += img.qrc
RC_FILE = CI4prev.rc
