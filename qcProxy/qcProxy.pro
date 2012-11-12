#-------------------------------------------------
#
# Project created by QtCreator 2012-10-16T23:42:57
#
#-------------------------------------------------

QT       += core network xml script

QT       -= gui

TARGET = qcProxy
CONFIG   += console serialport
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp \
    SerialDeviceConnector.cpp \
    GuiConnectionManager.cpp \
    DeviceStateManager.cpp \
    DeviceConnectionManagerBase.cpp \
    DeviceAPIFileHandler.cpp \
    DeviceAPI.cpp \
    Device.cpp \
    ConnectionServer.cpp \
    ProxySettingsManager.cpp \
    QcProxy.cpp \
    DeviceCommandBuilder.cpp \
    DummySocketDevice.cpp

HEADERS += \
    SerialDeviceConnector.h \
    GuiConnectionManager.h \
    DeviceStateManager.h \
    DeviceConnectionManagerBase.h \
    DeviceAPIFileHandler.h \
    DeviceAPI.h \
    Device.h \
    ConnectionServer.h \
    ProxySettingsManager.h \
    QcProxy.h \
    DeviceCommandBuilder.h \
    DummySocketDevice.h


CONFIG(debug, debug|release):unix:!macx:!symbian: LIBS += -lSerialPortd
CONFIG(release, debug|release):unix:!macx:!symbian: LIBS += -lSerialPort

INCLUDEPATH += $$PWD/../QSerialDevice/include
DEPENDPATH += $$PWD/../QSerialDevice/include

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
