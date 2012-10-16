
QT       += xml script

QT       -= gui

TARGET = qcCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += SettingsManagerBase.cpp \
    DeviceStateVariable.cpp \
    StateManagerBase.cpp \
    DeviceCommandBase.cpp \
    ClientCommandBase.cpp \
    ClientPacket.cpp \
    ClientCommandDevice.cpp \
    ClientConnectionManagerBase.cpp \
    DeviceAPIParser.cpp \
    ErrorHandlerBase.cpp

HEADERS += SettingsManagerBase.h \
    DeviceStateVariable.h \
    StateManagerBase.h \
    DeviceCommandBase.h \
    ClientCommandBase.h \
    ClientPacket.h \
    ClientCommandDevice.h \
    ClientConnectionManagerBase.h \
    DeviceAPIParser.h \
    ErrorHandlerBase.h

OTHER_FILES += \
    ../TODO
