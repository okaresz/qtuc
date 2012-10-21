#include "SerialDeviceConnector.h"
#include "ProxySettingsManager.h"

using namespace QtuC;
using namespace QtAddOn::SerialPort;

QtuC::SerialDeviceConnector::SerialDeviceConnector( QObject *parent ) : DeviceConnectionManagerBase( parent )
{
	mSerialPort = new SerialPort(this);
	connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(receivePart()));
}

QtuC::SerialDeviceConnector::~SerialDeviceConnector()
{
	this->closePort();
}

bool SerialDeviceConnector::sendCommand(const DeviceCommand &cmd)
{
	if( !mSerialPort->isOpen() )
	{
		error( QtWarningMsg, "Serial port is closed, sendCommand failed", "sendCommand()" );
		return false;
	}

	if( mSerialPort->write( cmd.getCommandString() ) <= 0 )
	{
		errorDetails_t errDet;
		errDet.insert( "cmdStr", cmd.getCommandString() );
		errDet.insert( "serialPort error", mSerialPort->errorString() );
		error( QtWarningMsg, "Failed to send command to device.", "senmdCommand()", errDet );
	}
	else
		{ debug( debugLevelVerbose, QString("Command sent on serial: %1").arg(cmd.getCommandString()) ); }
}

void SerialDeviceConnector::receivePart()
{
	QByteArray block;
	char c=0;
	bool fullCmd = false;
	while( mSerialPort->getChar(&c) )
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

		DeviceCommand cmd = buildCommand( mCmdRxBuffer );
		if( cmd )
			{ emit commandReceived(cmd); }
		else
		{ error( QtWarningMsg, "Invalid device command received, command dropped", "receivePart()"); }
		mCmdRxBufferShadow.clear();
	}
}

void SerialDeviceConnector::closePort()
{
	if( mSerialPort->isOpen() )
	{
		mSerialPort->close();
		debug( debugLevelInfo, "Serial port closed.", "closePort()" );
	}
}

bool QtuC::SerialDeviceConnector::connectPort()
{
	mSerialPort->setPort( ProxySettingsManager::instance()->value( "devicePort/portName" ); );

	QString serialBaud = ProxySettingsManager::instance()->value( "devicePort/baudRate" );

	if( mSerialPort->open( QIODevice::ReadWrite | QIODevice::Unbuffered ) )
	{
		if( !mSerialPort->setRate( serialBaud.toInt() ) )
		{
			error( QtCriticalMsg, QString( "Setting baud rate %1 failed" ).arg( serialBaud ), "connectPort()" );
			return false;
		}

		if( !mSerialPort->setDataBits( SerialPort::Data8 ) )
		{
			error( QtCriticalMsg, QString( "Setting bit count %1 failed" ).arg( SerialPort::Data8 ), "connectPort()" );
			return false;
		}

		if( !mSerialPort->setParity( SerialPort::NoParity ) )
		{
			error( QtCriticalMsg, QString( "Setting parity %1 failed" ).arg( SerialPort::NoParity ), "connectPort()" );
			return false;
		}

		if( !mSerialPort->setStopBits( SerialPort::OneStop ) )
		{
			error( QtCriticalMsg, QString( "Setting stop bits: %1 failed" ).arg( SerialPort::OneStop ), "connectPort()" );
			return false;
		}

		if( !mSerialPort->setFlowControl( SerialPort::NoFlowControl ) )
		{
			error( QtCriticalMsg, QString( "Setting flow control %1 failed" ).arg( SerialPort::NoFlowControl ), "connectPort()" );
			return false;
		}

		debug( "Serial port opened", "connectPort()" );
	}
	else
	{
		error( QtCriticalMsg, QString( "Opening serial port %1 failed" ).arg( ProxySettingsManager::instance()->value( "devicePort/portName" ) ), "connectPort()" );
		return false;
	}
	return true;
}

void SerialDeviceConnector::closeDevice()
{
	closePort();
}

bool SerialDeviceConnector::openDevice()
{
	connectPort();
}
