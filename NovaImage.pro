# ----------------------------------------------------
# This file is generated by the Qt Visual Studio Tools.
# ------------------------------------------------------
QT       += core gui widgets printsupport charts xml

CONFIG += qt thread debug c++11

TARGET = NovaImage
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

OBJECTS_DIR += debug
DESTDIR = ./bin

win32{
INCLUDEPATH += "C:/Program Files/DCMTK/include"
INCLUDEPATH += "C:/Program Files/FFTW"
LIBS += -L"C:/Program Files/DCMTK/lib" -ldcmtk
LIBS += -L"C:/Program Files/FFTW" -llibfftw3-3
}

unix{
INCLUDEPATH += "/home/devon/dcmtk-3.6.6/install/include"
INCLUDEPATH += "/usr/local/include"
LIBS += -L"/home/devon/dcmtk-3.6.6/install/lib" -ldcmtk
LIBS += /usr/local/lib/libfftw3.a
}

DEPENDPATH += .
MOC_DIR += ./GeneratedFiles/$(ConfigurationName)
UI_DIR += ./GeneratedFiles
RCC_DIR += ./GeneratedFiles
include(NovaImage.pri)

DEFINES  -= UNICODE
DEFINES  += UMBCS

QMAKE_RPATHDIR += ../lib