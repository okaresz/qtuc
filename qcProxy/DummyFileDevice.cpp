#include "DummyFileDevice.h"

using namespace QtuC;

DummyFileDevice::DummyFileDevice(QObject *parent) :
	DeviceConnectionManagerBase(parent)
{
	mDeviceFile = new QFile( "dummyDevice", this );
	connect(mDeviceFile, SIGNAL(readyRead()), this, SLOT(receivePart()));
}

DummyFileDevice::~DummyFileDevice()
{
	mDeviceFile->close();
}

bool DummyFileDevice::sendCommand(const DeviceCommand &cmd)
{
	if( !mDeviceFile->isOpen() )
	{
		error( QtWarningMsg, "Dummyd evice file is closed, sendCommand failed", "sendCommand()" );
		return false;
	}

	if( mDeviceFile->write( cmd.getCommandString().toStdString().c_str() ) <= 0 )
	{
		errorDetails_t errDet;
		errDet.insert( "cmdStr", cmd.getCommandString() );
		errDet.insert( "file IO error", mDeviceFile->errorString() );
		error( QtWarningMsg, "Failed to send command to device.", "senmdCommand()", errDet );
		return false;
	}
	else
		{ debug( debugLevelVeryVerbose, QString("Command wrote to file: %1").arg(cmd.getCommandString()), "sendCommand()" ); }
	return true;
}

void DummyFileDevice::receivePart()
{
	QByteArray block;
	char c=0;
	bool fullCmd = false;
	while( mDeviceFile->getChar(&c) )
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

		DeviceCommand *cmd = DeviceCommand::fromString( mCmdRxBuffer );
		if( cmd )
			{ emit commandReceived(cmd); }
		else
			{ error( QtWarningMsg, "Invalid device command received, command dropped", "receivePart()"); }
		mCmdRxBufferShadow.clear();
	}
}

void DummyFileDevice::closeFile()
{
	if( mDeviceFile->isOpen() )
	{
		mDeviceFile->close();
		debug( debugLevelInfo, "Dummy device file closed.", "closePort()" );
	}
}

bool DummyFileDevice::openFile()
{
	if( mDeviceFile->open( QIODevice::ReadWrite ) )
	{
		debug( debugLevelInfo, "Dummy device file opened", "openFile()" );
	}
	else
	{
		errorDetails_t errDet;
		errDet.insert( "error message", mDeviceFile->errorString() );
		error( QtCriticalMsg, "Opening dummy device file failed", "openFile()", errDet );
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
	return openFile();
}
