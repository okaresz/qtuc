#include "ClientConnectionManagerBase.h"
#include "ClientCommandBase.h"
#include <QCoreApplication>

using namespace QtuC;

int ClientConnectionManagerBase::mInstanceCount = 0;
QHash<QString,QString> ClientConnectionManagerBase::mSelfInfo = QHash<QString,QString>();
ClientCommandFactory *ClientConnectionManagerBase::mCommandFactory = 0;

ClientConnectionManagerBase::ClientConnectionManagerBase( QTcpSocket *socket, bool isServerRole, QObject *parent ) :
	ErrorHandlerBase(parent),
	mState(connectionUnInitialized),
	mServerRole(isServerRole),
	mHeartBeatCount(0),
	mClientSocket(socket)
{
	++mInstanceCount;
	if( mClientSocket->isOpen() )
	{
		if( !mCommandFactory )
		{
			mCommandFactory = new ClientCommandFactory(this);
			ClientPacket::setCommandFactoryPtr( mCommandFactory );
		}

		if( !mSelfInfo.contains("id") )
		{
			debug( debugLevelVerbose, "Self-info is not set for ClientConnectionManagerBase! Call ClientConnectionManagerBase::setSelfInfo() before instantiation", "ClientConnectionManagerBase()" );
			// set something, because this is required for communication; this will be overwritten from settings
			QHash<QString,QString> minimalSelfInfo;
			QString fallBackId = QCoreApplication::instance()->applicationName();
			if( fallBackId.isEmpty() )
				{ fallBackId = QString("unknown_client"); }
			else
				{ fallBackId = QString( fallBackId.simplified().trimmed().replace(' ', '_').toAscii() ); }
			minimalSelfInfo.insert( QString("id"), fallBackId );
			setSelfInfo(minimalSelfInfo);
		}
		ClientPacket::setSelfId( mSelfInfo.value(QString("id")) );

		// set some socket params, fine-tune connection...
		setState( connectionConnected );
		mClientSocket->setSocketOption( QAbstractSocket::LowDelayOption, 1 );
		//mClientSocket->setReadBufferSize(80);	//~one Command

		connect( mClientSocket, SIGNAL(readyRead()), this, SLOT(receiveClientData()) );
		connect( this, SIGNAL(packetReceived(ClientPacket*)), this, SLOT(handleReceivedPacket(ClientPacket*)) );

		/// @todo Am I sure to handle it locally like this?
		connect(mClientSocket, SIGNAL(disconnected()), this, SLOT(handleDisconnected()));

		debug( debugLevelInfo, "New client socket connected", "ClientConnectionManagerBase()" );
	}
	else
	{ error( QtWarningMsg, "Huh? Got an empty socket!?", "ClientConnectionManagerBase()" ); }
}

ClientConnectionManagerBase::~ClientConnectionManagerBase()
{
	debug( debugLevelVerbose, "Disconnect client before closing...", "~ClientConnectionManagerBase()" );
	if( mClientSocket )
		{ mClientSocket->close(); }
	if( --mInstanceCount == 0 )
	{
		delete mCommandFactory;
	}
}

bool ClientConnectionManagerBase::isConnected() const
{
	return ( mState == connectionConnected || mState == connectionHandShaking || mState == connectionReady );
}

bool ClientConnectionManagerBase::isReady() const
{
	return ( mState == connectionReady );
}

const QString ClientConnectionManagerBase::getID() const
{
	return mClientInfo.value("id");
}

connectionState_t ClientConnectionManagerBase::getState() const
{
	return mState;
}

const QHash<QString,QString> ClientConnectionManagerBase::getClientInfo() const
{
	return mClientInfo;
}

const QHash<QString,QString> ClientConnectionManagerBase::getSelfInfo() const
{
	return mSelfInfo;
}

const QString ClientConnectionManagerBase::getSelfInfo(const QString &infoName) const
{
	return mSelfInfo.value( infoName, QString() );
}

void ClientConnectionManagerBase::setSelfInfo( const QString &key, const QString &value )
{
	if( !value.isEmpty() )
		{ mSelfInfo.insert( key, value ); }
	ClientPacket::setSelfId( mSelfInfo.value(QString("id")) );
}

bool ClientConnectionManagerBase::setSelfInfo(const QHash<QString, QString> &infoList)
{
	if( infoList.value("id", QString() ).isEmpty() )
	{
		ErrorHandlerBase::error( QtWarningMsg, "Id is missing from selfInfo list", "setSelfInfo()", "ClientConnectionManagerBase" );
		return false;
	}
	mSelfInfo = infoList;
	ClientPacket::setSelfId( mSelfInfo.value(QString("id")) );
	return true;
}

bool ClientConnectionManagerBase::sendCommand( ClientCommandBase *cmd )
{
	if( !cmd->isValid() )
	{
		error( QtWarningMsg, QString("ClientCommand (%1) is invalid").arg(cmd->getName()), "sendCommand()" );
		return false;
	}
	ClientPacket *packet = new ClientPacket( cmd );
	return sendPacket( packet );
}

bool ClientConnectionManagerBase::sendCommands( const QList<ClientCommandBase*> &cmdList )
{
	if( cmdList.isEmpty() )
	{
		error( QtWarningMsg, "Command list is empty", "sendCommands()" );
		return false;
	}

	ClientPacket *packet = new ClientPacket( cmdList.at(0) );
	for( int i=1; i<cmdList.size(); ++i )
	{
		packet->appendCommand( cmdList.at(i) );
	}

	return sendPacket( packet );
}

bool ClientConnectionManagerBase::sendPacket(ClientPacket *packet)
{
	if( packet->isValid() )
	{
		if( !checkSocket() )
		{
			error( QtWarningMsg, "Client socket is not ready, checkSocket() failed", "sendPacket()" );
			packet->deleteLater();
			return false;
		}
		if( mClientSocket->write( packet->getPacketData() ) < 0 )
		{
			error( QtWarningMsg, "Error during sending client packet", "sendPacket()" );
			packet->deleteLater();
			return false;
		}
		else
		{
			packet->deleteLater();
			return true;
		}
	}
	else
	{
		error( QtWarningMsg, "Packet is invalid", "sendPacket()" );
		packet->deleteLater();
		return false;
	}
}

bool ClientConnectionManagerBase::sendHandShake()
{
	setState( connectionHandShaking );
	ClientCommandHandshake *hs = new ClientCommandHandshake( mSelfInfo );
	if( !sendCommand( hs ) )
	{
		error( QtCriticalMsg, "Failed to send handshake", "sendHandShake()" );
		return false;
	}
	else
	{
		debug( debugLevelVeryVerbose, "Sending initial handshake...", "sendHandShake()" );
		return true;
	}
}

void ClientConnectionManagerBase::receiveClientData()
{
	while(1)
	{
		if( !checkSocket() )
		{
			error( QtWarningMsg, "Client socket is not ready, checkSocket() failed", "receiveClientData()" );
			return;
		}

		//is it worth to read?
		///  @todo It's not the responsibility ofClientConnectionManagerBase to know how big packet is needed... clean up this sizeof thing from here...
		quint16 minPackeSize = sizeof(quint16)+1;
		if( mClientSocket->peek(minPackeSize).size() < minPackeSize )	//2bytes of packetSize + 1 is the minimum size
			{ return; }

		// read packetSize
		quint16 packetSize = ClientPacket::readPacketSize( mClientSocket->peek(sizeof(quint16)) );
		// has the whole packet arrived?
		if( mClientSocket->peek(packetSize+sizeof(quint16)).size() < packetSize+sizeof(quint16) )
			{ return; }

		ClientPacket *packet = ClientPacket::fromPacketData( mClientSocket->read( packetSize+sizeof(quint16) ) );
		if( !packet )
		{
			error( QtWarningMsg, "Failed to create ClientPacket from data", "receiveClientData()" );
			continue;
		}

		if( packet->isValid() )
			{ emit packetReceived( packet ); }
		else
		{
			error( QtWarningMsg, "Received packet is invalid", "receiveClientData()" );
			continue;
		}
	}
}

void ClientConnectionManagerBase::handleReceivedPacket(ClientPacket *packet)
{
	const QList<ClientCommandBase*> packetCommands = packet->getCommands();
	for( int i=0; i<packetCommands.size(); ++i )
	{
		if( packetCommands.at(i)->isValid() )
		{
			ClientCommandBase *cmd = packet->detachCommand(i);
			if( !reflex( cmd ) )
				{ emit commandReceived( cmd ); }
		}
		else
		{
			error( QtWarningMsg, QString("Received command (%1) is invalid, dropped").arg(packetCommands.at(i)->getName()), "handleReceivedPacket()" );
			delete packet->detachCommand(i);
		}
	}
	packet->destroyShell();
}

bool ClientConnectionManagerBase::reflex( ClientCommandBase *command )
{
	if( command->getName() == "heartBeat" )
	{
		replyHeartBeat( (ClientCommandHeartBeat*)command );
		return true;
	}
	else if( command->getName() == "handshake" )
	{
		ackHandShake( (ClientCommandHandshake*)command );
		return true;
	}
	return false;
}

void ClientConnectionManagerBase::replyHeartBeat( ClientCommandHeartBeat* incomingHeartBeat )
{
	ClientCommandHeartBeat *reply = incomingHeartBeat->cloneReply();
	sendCommand( reply );
	incomingHeartBeat->deleteLater();
}

void ClientConnectionManagerBase::ackHandShake( ClientCommandHandshake *handshake )
{
	if( mServerRole )
	{
		if( handshake->isValid() )
		{
			if( mState != connectionHandShaking )	//first hs received, reply with our info
			{
				setState( connectionHandShaking );
				mClientInfo = handshake->getInfo();
				ClientCommandHandshake *replyHs = new ClientCommandHandshake(mSelfInfo, true);
				sendCommand( replyHs );
				debug( debugLevelVeryVerbose, "Client handshake received, reply with ACK handshake...", "ackHandShake()" );
			}
			else	//second hs, an empty ack?
			{
				if( handshake->isAck() && handshake->getAck() )
				{
					setState( connectionReady );
					debug( debugLevelInfo, "Client handshake successful, client is ready.", "ackHandShake()" );
				}
				else
					{ error( QtWarningMsg, "Client sent a false ACK or a non-ACK handshake, state remains handshaking.", "ackHandShake()" ); }
			}
		}
	}
	else
	{
		if( mState == connectionHandShaking )
		{
			if( handshake->isValid() )
			{
				mClientInfo = handshake->getInfo();
				ClientCommandHandshake *replyHs = new ClientCommandHandshake(true);
				sendCommand( replyHs );
				debug( debugLevelInfo, "Handshake successful, connected.", "ackHandShake()" );
				setState( connectionReady );
			}
			//handleReceivedPacket() only pass valid commands
		}
		else
			{ error( QtWarningMsg, "Received handshake, but never sent one. Misbehaving server?... connection state unchanged.", "ackHandShake()" ); }
	}
	handshake->deleteLater();
}

void ClientConnectionManagerBase::handleDisconnected()
{
	setState( connectionDisconnected );
	emit clientDisconnected();
}

bool ClientConnectionManagerBase::checkSocket()
{
	return ( mClientSocket &&
			mClientSocket->isOpen() &&
			mClientSocket->isReadable() &&
			mClientSocket->isWritable()
			 );
}

void ClientConnectionManagerBase::setState(connectionState_t newState)
{
	mState = newState;
	emit connectionStateChanged( newState );
	switch( newState )
	{
		case connectionUndefined:
		case connectionUnInitialized: break;
		case connectionConnected: emit connectionStateConnected(); break;
		case connectionHandShaking: emit connectionStateHandShaking(); break;
		case connectionReady: emit connectionStateReady(); break;
		case connectionLost: emit connectionStateLost(); break;
		case connectionDisconnected: emit connectionStateDisconnected();  break;
		case connectionError: emit connectionStateError();  break;
	}
}
