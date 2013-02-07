#include "SerialDeviceConnector.h"
#include "ProxySettingsManager.h"

using namespace QtuC;
using namespace QtAddOn::SerialPort;

SerialDeviceConnector::SerialDeviceConnector( QObject *parent ) : DeviceConnectionManagerBase( parent )
{
	mSerialPort = new SerialPort(this);
	connect(mSerialPort, SIGNAL(readyRead()), this, SLOT(receivePart()));
}

SerialDeviceConnector::~SerialDeviceConnector()
{
	this->closeDevice();
}

bool SerialDeviceConnector::sendCommand( DeviceCommand *cmd )
{
	if( !cmd )
		{ return false; }

	if( !cmd->isValid() )
	{
		error( QtWarningMsg, "Command is invalid, cannot be sent.", "sendCommand()" );
		cmd->deleteLater();
		return false;
	}

	if( !mSerialPort->isOpen() )
	{
		error( QtWarningMsg, "Serial port is closed, sendCommand failed", "sendCommand()" );
		cmd->deleteLater();
		return false;
	}

	// get commandString once
	QByteArray commandString = cmd->getCommandString().toAscii();
	if( mSerialPort->write( commandString ) <= 0 )
	{
		errorDetails_t errDet;
		errDet.insert( "cmdStr", QString(commandString) );
		errDet.insert( "serialPort error", mSerialPort->errorString() );
		error( QtWarningMsg, "Failed to send command to device.", "senmdCommand()", errDet );
		cmd->deleteLater();
		return false;
	}
	else
		{ debug( debugLevelVeryVerbose, QString("Command sent on serial: %1").arg(cmd->getCommandString()), "sendCommand()" ); }
	cmd->deleteLater();
	return true;
}

void SerialDeviceConnector::receivePart()
{
	QByteArray block;
	char c=0;
	bool fullCmd = false;
	while( mSerialPort->getChar(&c) )
	{
		if( c && c != '\n' && c != '\r' )
		{
			block.append(c);
		}
		if( c == '\n' )
		{
			fullCmd = true;
			break;
		}
	}

	if( !block.isEmpty() )
		{ mCmdRxBuffer.append(block); }

	if( fullCmd )
	{
		debug( debugLevelVeryVerbose, QString("Command received on serial: %1").arg(mCmdRxBuffer), "receivePart()" );

		DeviceCommand *cmd = DeviceCommand::fromString( mCmdRxBuffer );
		if( cmd )
			{ emit commandReceived((DeviceCommand*)cmd); }
		else
			{ error( QtWarningMsg, "Invalid device command received, command dropped", "receivePart()"); }
		mCmdRxBuffer.clear();
	}
}

void SerialDeviceConnector::closeDevice()
{
	if( mSerialPort->isOpen() )
	{
		mSerialPort->close();
		debug( debugLevelInfo, "Serial port closed.", "closePort()" );
	}
}

bool SerialDeviceConnector::openDevice()
{
    mSerialPort->setPort( ProxySettingsManager::instance()->value( "devicePort/portName" ).toString() );
	QString serialBaud = ProxySettingsManager::instance()->value( "devicePort/baudRate" ).toString();
    if( mSerialPort->open( QIODevice::ReadWrite ) )
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

		debug( debugLevelInfo, QString("Serial port %1 opened").arg(ProxySettingsManager::instance()->value( "devicePort/portName" ).toString()), "connectPort()" );
	}
	else
	{
		errorDetails_t errDet;
		errDet.insert( "error message", mSerialPort->errorString() );
		error( QtCriticalMsg, QString( "Opening serial port %1 failed" ).arg( ProxySettingsManager::instance()->value( "devicePort/portName" ).toString() ), "connectPort()", errDet );
		return false;
	}
	return true;
}
