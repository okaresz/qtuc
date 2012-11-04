#-------------------------------------------------
#
# Project created by QtCreator 2012-11-02T01:13:06
#
#-------------------------------------------------

QT       += core gui network xml script

TARGET = qcGUI
TEMPLATE = app


SOURCES += main.cpp\
    QcGui.cpp \
    QcGuiMainWindow.cpp \
    ProxyStateManager.cpp \
    ProxyConnectionManager.cpp \
    GuiSettingsManager.cpp

HEADERS  += \
    QcGui.h \
    QcGuiMainWindow.h \
    ProxyStateManager.h \
    ProxyConnectionManager.h \
    GuiSettingsManager.h

FORMS    += \
    QcGuiMainWindow.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommond
else:symbian: LIBS += -lqcCommon
else:unix: LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommon

INCLUDEPATH += $$PWD/../qcCommon
INCLUDEPATH += $$PWD/../qcCommon/clientCommands
DEPENDPATH += $$PWD/../qcCommon

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/qcCommon.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/qcCommond.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/libqcCommon.a
