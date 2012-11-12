#include "DummySocketDevice.h"
#include "ProxySettingsManager.h"

using namespace QtuC;

DummySocketDevice::DummySocketDevice(QObject *parent) :
	DeviceConnectionManagerBase(parent)
{
	connect( this, SIGNAL(quitRequestFromDevice()), this, SLOT(handleQuitRequestFromDevice()) );

	mDeviceSocket = new QTcpSocket(this);
	mDeviceServer = new QTcpServer(this);
	connect( mDeviceServer, SIGNAL(newConnection()), this, SLOT(handleDeviceClientConnected()) );
}

DummySocketDevice::~DummySocketDevice()
{
	mDeviceSocket->close();
}

bool DummySocketDevice::sendCommand( DeviceCommandBuilder *cmd )
{
	if( !cmd )
		{ return false; }

	mDeviceSocket->write( cmd->getCommandString().toLocal8Bit().data() );

	//debug( debugLevelVeryVerbose, QString("Command wrote to file: %1").arg(cmd->getCommandString()), "sendCommand()" );
	cmd->deleteLater();
	return true;
}

void DummySocketDevice::handleDeviceData()
{
	if( !mDeviceSocket->isReadable() )
	{
		error( QtWarningMsg, "Dummy device socket is not ready, reading failed", "handleDeviceData()" );
		return;
	}

	QByteArray block;
	char c=0;
	bool fullCmd = false;
	while( mDeviceSocket->getChar(&c) )
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
		if( mCmdRxBufferShadow.trimmed() == "quit" )
		{
			emit quitRequestFromDevice();
			mCmdRxBufferShadow.clear();
			return;
		}
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

void DummySocketDevice::handleSocketError(QAbstractSocket::SocketError errorNum)
{
	error( QtWarningMsg, QString("Dummy device socket error %1: %2").arg(QString::number((int)errorNum), ((QIODevice*)sender())->errorString()), "handleSocketError()" );
}

void DummySocketDevice::handleDeviceClientConnected()
{
	mDeviceSocket = mDeviceServer->nextPendingConnection();
	connect( mDeviceSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(handleSocketError(QAbstractSocket::SocketError)) );
	connect( mDeviceSocket, SIGNAL(readyRead()), this, SLOT(handleDeviceData()) );
	debug( debugLevelInfo, "Dummy device socket connected", "handleDeviceClientConnected()" );
}

void DummySocketDevice::handleQuitRequestFromDevice()
{
	mDeviceSocket->close();
	debug( debugLevelInfo, "Dummy device socket disconnected", "closeFile()" );
}

void DummySocketDevice::closeDevice()
{
	mDeviceSocket->close();
	debug( debugLevelInfo, "Dummy device socket disconnected", "closeFile()" );
}

bool DummySocketDevice::openDevice()
{
	QString host = ProxySettingsManager::instance()->value("dummyDeviceSocket/host").toString();
	if( host == "localhost" )
		{ host = "127.0.0.1"; }

	if( mDeviceServer->listen( QHostAddress(host), ProxySettingsManager::instance()->value("dummyDeviceSocket/port").toInt() ) )
	{
		debug( debugLevelVerbose, QString("Listening for dummy device on %1, port %2.").arg( ProxySettingsManager::instance()->value("dummyDeviceSocket/host").toString(), ProxySettingsManager::instance()->value("dummyDeviceSocket/port").toString() ), "openDevice()" );
		return true;
	}
	else
	{
		errorDetails_t errDet;
		errDet.insert( "error", mDeviceServer->errorString() );
		error( QtCriticalMsg, QString("Listen for dummy device on %1, port %2").arg( ProxySettingsManager::instance()->value("dummyDeviceSocket/host").toString(), ProxySettingsManager::instance()->value("dummyDeviceSocket/port").toString() ), "openDevice()", errDet );
		return false;
	}
}
