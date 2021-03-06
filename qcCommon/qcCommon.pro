
QT       += xml script network

QT       -= gui

TARGET = qcCommon
TEMPLATE = lib
CONFIG += staticlib

SOURCES += SettingsManagerBase.cpp \
	DeviceStateVariableBase.cpp \
    StateManagerBase.cpp \
    DeviceCommandBase.cpp \
    ClientCommandBase.cpp \
    ClientPacket.cpp \
    ClientCommandDevice.cpp \
    ClientConnectionManagerBase.cpp \
    DeviceAPIParser.cpp \
    ErrorHandlerBase.cpp \
	QCommandLine.cpp \
    ClientCommandFactory.cpp \
    clientCommands/ClientCommandHeartBeat.cpp \
	clientCommands/ClientCommandHandshake.cpp \
    clientCommands/ClientCommandDeviceApi.cpp \
    clientCommands/ClientCommandReqDeviceApi.cpp \
    clientCommands/ClientCommandUnSubscribe.cpp \
	clientCommands/ClientCommandSubscribe.cpp \
    clientCommands/ClientCommandReqDeviceInfo.cpp \
    clientCommands/ClientCommandDeviceInfo.cpp

HEADERS += SettingsManagerBase.h \
	DeviceStateVariableBase.h \
    StateManagerBase.h \
    DeviceCommandBase.h \
    ClientCommandBase.h \
    ClientPacket.h \
    ClientCommandDevice.h \
    ClientConnectionManagerBase.h \
    DeviceAPIParser.h \
	ErrorHandlerBase.h \
	QCommandLine.h \
    ClientCommandFactory.h \
    clientCommands/ClientCommands.h \
    clientCommands/ClientCommandHeartBeat.h \
	clientCommands/ClientCommandHandshake.h \
    clientCommands/ClientCommandDeviceApi.h \
    clientCommands/ClientCommandReqDeviceApi.h \
    clientCommands/ClientCommandUnSubscribe.h \
	clientCommands/ClientCommandSubscribe.h \
    clientCommands/ClientCommandReqDeviceInfo.h \
    clientCommands/ClientCommandDeviceInfo.h

INCLUDEPATH += $$PWD/clientCommands
