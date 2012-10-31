
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
    ErrorHandlerBase.cpp \
    ClientCommandFactory.cpp \
    ClientCommandHeartBeat.cpp

HEADERS += SettingsManagerBase.h \
    DeviceStateVariable.h \
    StateManagerBase.h \
    DeviceCommandBase.h \
    ClientCommandBase.h \
    ClientPacket.h \
    ClientCommandDevice.h \
    ClientConnectionManagerBase.h \
    DeviceAPIParser.h \
    ErrorHandlerBase.h \
    ClientCommandFactory.h \
    ClientCommandHeartBeat.h \
    ClientCommands.h

OTHER_FILES +=
