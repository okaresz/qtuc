#-------------------------------------------------
#
# Project created by QtCreator 2012-10-16T23:42:57
#
#-------------------------------------------------

QT       += core network xml

QT       -= gui

TARGET = qcProxy
CONFIG   += console serialport
CONFIG   -= app_bundle

#for static linking
#CONFIG   += link_prl

TEMPLATE = app


SOURCES += main.cpp \
    SerialDeviceConnector.cpp \
    GuiConnectionManager.cpp \
    DeviceStateManager.cpp \
    DeviceConnectionManagerBase.cpp \
    DeviceCommand.cpp \
    DeviceAPIFileHandler.cpp \
    DeviceAPI.cpp \
    Device.cpp \
    ConnectionServer.cpp \
    ProxySettingsManager.cpp

HEADERS += \
    SerialDeviceConnector.h \
    GuiConnectionManager.h \
    DeviceStateManager.h \
    DeviceConnectionManagerBase.h \
    DeviceCommand.h \
    DeviceAPIFileHandler.h \
    DeviceAPI.h \
    Device.h \
    ConnectionServer.h \
    ProxySettingsManager.h


CONFIG(debug, debug|release):unix:!macx:!symbian: LIBS += -L$$PWD/../QSerialDevice/lib/ -lSerialPortd-qt481-x86_64
CONFIG(release, debug|release):unix:!macx:!symbian: LIBS += -L$$PWD/../QSerialDevice/lib/ -lSerialPort-qt481-x86_64

INCLUDEPATH += $$PWD/../QSerialDevice/include
DEPENDPATH += $$PWD/../QSerialDevice/include
