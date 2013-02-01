
QT       += xml script network

QT       -= gui

TARGET = qcCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += SettingsManagerBase.cpp \
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
	clientCommands/ClientCommandHandshake.cpp \
    clientCommands/ClientCommandDeviceApi.cpp \
    clientCommands/ClientCommandReqDeviceApi.cpp \
    clientCommands/ClientCommandUnSubscribe.cpp \
    clientCommands/ClientCommandSubscribe.cpp \
    DeviceStateVariableBase.cpp

HEADERS += SettingsManagerBase.h \
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
	clientCommands/ClientCommandHandshake.h \
    clientCommands/ClientCommandDeviceApi.h \
    clientCommands/ClientCommandReqDeviceApi.h \
    clientCommands/ClientCommandUnSubscribe.h \
    clientCommands/ClientCommandSubscribe.h \
    DeviceStateVariableBase.h

INCLUDEPATH += $$PWD/clientCommands
