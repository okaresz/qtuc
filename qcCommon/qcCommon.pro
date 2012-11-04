
QT       += xml script network

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
    clientCommands/ClientCommandHeartBeat.cpp \
    clientCommands/ClientCommandHandshake.cpp

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
    clientCommands/ClientCommands.h \
    clientCommands/ClientCommandHeartBeat.h \
    clientCommands/ClientCommandHandshake.h

INCLUDEPATH += $$PWD/clientCommands
