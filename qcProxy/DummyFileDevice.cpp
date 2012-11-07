#include "DummyFileDevice.h"
#include <iostream>

using namespace QtuC;

DummyFileDevice::DummyFileDevice(QObject *parent) :
	DeviceConnectionManagerBase(parent)
{
	mSocketNotifier = new QSocketNotifier( 0, QSocketNotifier::Read, this);
	connect( mSocketNotifier, SIGNAL(activated(int)), this, SLOT(receivePart()) );
	mDeviceInFile = new QFile( this );
	//connect(mDeviceFile, SIGNAL(readyRead()), this, SLOT(receivePart()));
}

DummyFileDevice::~DummyFileDevice()
{
	mDeviceInFile->close();
	mSocketNotifier->deleteLater();
}

bool DummyFileDevice::sendCommand( DeviceCommandBuilder *cmd )
{
	if( !cmd )
		{ return false; }

	std::cout << cmd->getCommandString().toStdString();

	//debug( debugLevelVeryVerbose, QString("Command wrote to file: %1").arg(cmd->getCommandString()), "sendCommand()" );
	cmd->deleteLater();
	return true;
}

void DummyFileDevice::receivePart()
{ qDebug("recpart");
	QByteArray block;
	char c=0;
	bool fullCmd = false;
	while( mDeviceInFile->getChar(&c) )
	{
		if( c && c != '\n' )
		{
			block.append(c);
		}
		if( c == '\n' )
		{
			fullCmd = true;
			break;
		}
	}

	if( block.isEmpty() )
		{ fullCmd = false; }
	else
		{ mCmdRxBufferShadow.append(block); }

	if( fullCmd )
	{
		mCmdRxBuffer = mCmdRxBufferShadow;
		debug( debugLevelInfo, QString("Command received on serial: %1").arg(mCmdRxBuffer), "receivePart()" );

		DeviceCommandBuilder *cmd = DeviceCommandBuilder::fromString( mCmdRxBuffer );
		if( cmd )
			{ emit commandReceived((DeviceCommand*)cmd); }
		else
			{ error( QtWarningMsg, "Invalid device command received, command dropped", "receivePart()"); }
		mCmdRxBufferShadow.clear();
	}
}

void DummyFileDevice::closeFile()
{
	if( mDeviceInFile->isOpen() )
	{
		mDeviceInFile->close();
		debug( debugLevelInfo, "Dummy device file closed.", "closePort()" );
	}
}

bool DummyFileDevice::openInFile()
{
	if( mDeviceInFile->open( 0, QIODevice::ReadOnly ) )
	{
		debug( debugLevelInfo, "Dummy device in file opened", "openFile()" );
	}
	else
	{
		errorDetails_t errDet;
		errDet.insert( "error message", mDeviceInFile->errorString() );
		error( QtCriticalMsg, "Opening dummy device in file failed", "openFile()", errDet );
		return false;
	}
	return true;
}

void DummyFileDevice::closeDevice()
{
	closeFile();
}

bool DummyFileDevice::openDevice()
{
	return openInFile();
}
