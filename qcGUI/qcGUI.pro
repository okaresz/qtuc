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
    ProxyStateManager.cpp \
    ProxyConnectionManager.cpp \
    GuiSettingsManager.cpp \
    QcGuiMainView.cpp \
    StateVariablesView.cpp \
    StateVarIntView.cpp \
    StateVariableCustomViewBase.cpp \
    StateVariableUlongBinView.cpp

HEADERS  += \
    QcGui.h \
    ProxyStateManager.h \
    ProxyConnectionManager.h \
    GuiSettingsManager.h \
    QcGuiMainView.h \
    StateVariablesView.h \
    StateVarIntView.h \
    StateVariableCustomViewBase.h \
    StateVariableUlongBinView.h

FORMS    += \
    QcGuiMainView.ui

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/release/ -lqcCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/debug/ -lqcCommon
else:unix: LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommon

INCLUDEPATH += $$PWD/../qcCommon
INCLUDEPATH += $$PWD/../qcCommon/clientCommands
DEPENDPATH += $$PWD/../qcCommon

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/release/qcCommon.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/debug/qcCommon.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/libqcCommon.a
