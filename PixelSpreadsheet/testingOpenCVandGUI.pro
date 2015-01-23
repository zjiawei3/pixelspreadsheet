#-------------------------------------------------
#
# Project created by QtCreator 2011-01-31T20:48:37
#
#-------------------------------------------------

QT       += core gui

TARGET = testingOpenCVandGUI
TEMPLATE = app
RESOURCES = Resources.qrc

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h \
 #           cv.h \
 #   cxcore.h \
 #   cv.hpp \
 #   cvaux.h \
 #   cvaux.hpp \
 #   cvcompat.h \
#    cvinternal.h \
#    cvtypes.h \
#    cvver.h \
#    cvvidsurv.hpp \
#    cvwimage.h \
#    cxcore.hpp \
#    cxerror.h \
#    cxflann.h \
#    cxmat.hpp \
#    cxmisc.h \
#    cxoperations.hpp \
#    cxtypes.h \
#    highgui.h \
#    highgui.hpp \
#    ml.h \
    ui_mainwindow.h \
    main.h

FORMS    += mainwindow.ui

#LIBS    += libcv.2.1.0.dylib \
#        libcvaux.2.1.0.dylib \
#        libcxcore.2.1.0.dylib \
#        libhighgui.2.1.0.dylib \
#        libml.2.1.0.dylib


OTHER_FILES += test.jpg \
    ConverterSymbols.jpg \
    Reverse.jpg


#RC_FILE = myapp.rc

QMAKE_LFLAGS += -static-libgcc
#QMAKE_LFLAGS += -static-libstdc++
