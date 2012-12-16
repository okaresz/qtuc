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
    DummySocketDevice.cpp \
    ClientSubscription.cpp \
    ClientSubscriptionManager.cpp

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
    DummySocketDevice.h \
    ClientSubscription.h \
    ClientSubscriptionManager.h

# Config for QtSerialPort.
# On linux, ld must find the lib (no config), on win, use the one in the QtSerialPort dir
unix:CONFIG(debug, debug|release): LIBS += -lSerialPortd
else:unix:CONFIG(release, debug|release): LIBS += -lSerialPort
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../QtSerialPort/lib/win32 -lSerialPortd1
else:win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../QtSerialPort/lib/win32 -lSerialPort1


INCLUDEPATH += $$PWD/../QtSerialPort/include
DEPENDPATH += $$PWD/../QtSerialPort/include

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/release -lqcCommon
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../qcCommon/debug -lqcCommon
else:unix: LIBS += -L$$OUT_PWD/../qcCommon/ -lqcCommon

INCLUDEPATH += $$PWD/../qcCommon
INCLUDEPATH += $$PWD/../qcCommon/clientCommands
DEPENDPATH += $$PWD/../qcCommon

win32:CONFIG(release, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/release/qcCommon.lib
else:win32:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/debug/qcCommon.lib
else:unix:!symbian: PRE_TARGETDEPS += $$OUT_PWD/../qcCommon/libqcCommon.a
