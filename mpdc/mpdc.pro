QT       += core gui webenginewidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    Utility/DumpUtil.cpp \
    Utility/IcrCriticalSection.cpp \
    Utility/ImCharset.cpp \
    Utility/ImPath.cpp \
    Utility/LogBuffer.cpp \
    Utility/LogUtil.cpp \
    browserwindow.cpp \
    ccollectorbase.cpp \
    collectorfactory.cpp \
    collectstatusmanager.cpp \
    datamodel.cpp \
    debugdialog.cpp \
    douyincollector.cpp \
    kuaishoucollector.cpp \
    main.cpp \
    mainwindow.cpp \
    settingmanager.cpp \
    weibocollector.cpp \
    weixincollector.cpp

HEADERS += \
    Utility/DumpUtil.h \
    Utility/IcrCriticalSection.h \
    Utility/ImCharset.h \
    Utility/ImPath.h \
    Utility/LogBuffer.h \
    Utility/LogMacro.h \
    Utility/LogUtil.h \
    browserwindow.h \
    ccollectorbase.h \
    collectorfactory.h \
    collectstatusmanager.h \
    datamodel.h \
    debugdialog.h \
    douyincollector.h \
    kuaishoucollector.h \
    mainwindow.h \
    settingmanager.h \
    weibocollector.h \
    weixincollector.h

FORMS += \
    browserwindow.ui \
    debugdialog.ui \
    mainwindow.ui

# Enable PDB generation
QMAKE_CFLAGS_RELEASE += /Zi
QMAKE_CXXFLAGS_RELEASE += /Zi
QMAKE_LFLAGS_RELEASE += /DEBUG

# QXlsx code for Application Qt project
QXLSX_PARENTPATH=./QXlsx         # current QXlsx path is . (. means curret directory)
QXLSX_HEADERPATH=./QXlsx/header/  # current QXlsx header path is ./header/
QXLSX_SOURCEPATH=./QXlsx/source/  # current QXlsx source path is ./source/
include(./QXlsx/QXlsx.pri)

INCLUDEPATH += QXLSX_HEADERPATH

