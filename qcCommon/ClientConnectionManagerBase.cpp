#include "ClientConnectionManagerBase.h"
#include "ClientCommandBase.h"

using namespace QtuC;

QHash<QString,QString> ClientConnectionManagerBase::mSelfInfo = QHash<QString,QString>();

ClientConnectionManagerBase::ClientConnectionManagerBase( QTcpSocket *socket, QObject *parent ) : ErrorHandlerBase(parent), mState(connectionUnInitialized)
{
	mCommandFactory = new ClientCommandFactory(this);
	ClientPacket::setCommandFactoryPtr( mCommandFactory );

	mClientSocket = socket;
	if( mClientSocket->isOpen() )
	{
		mState = connectionConnected;
		mClientSocket->setSocketOption( QAbstractSocket::LowDelayOption, 1 );
		//mClientSocket->setReadBufferSize(80);	//~one Command

		connect( mClientSocket, SIGNAL(readyRead()), this, SLOT(receiveClientData()) );
		connect( this, SIGNAL(packetReceived(ClientPacket*)), this, SLOT(handleReceivedPacket(ClientPacket*)) );
		connect( this, SIGNAL(commandReceived(ClientCommandBase*)), this, SLOT(reflex(ClientCommandBase*)) );

		/// @todo Am I sure to handle it locally like this?
		connect(mClientSocket, SIGNAL(disconnected()), mClientSocket, SLOT(deleteLater()));

		debug( debugLevelInfo, "New client socket connected", "ClientConnectionManagerBase()" );
	}
	else
		{ error( QtWarningMsg, "Huh? Got an empty socket!?", "ClientConnectionManagerBase()" ); }
}

bool ClientConnectionManagerBase::isConnected() const
{
	return ( mState == connectionConnected || mState == connectionHandShaking || mState == connectionReady );
}

bool ClientConnectionManagerBase::isReady() const
{
	return ( mState == connectionReady );
}

const QString &ClientConnectionManagerBase::getID() const
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
		error( QtWarningMsg, "Id is missing from selfInfo list", "setSelfInfo()" );
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
	ClientPacket *packet = new ClientPacket( cmd, this );
	return sendPacket( packet );
}

bool ClientConnectionManagerBase::sendCommands( const QList<ClientCommandBase*> &cmdList )
{
	if( cmdList.isEmpty() )
	{
		error( QtWarningMsg, "Command list is empty", "sendCommands()" );
		return false;
	}

	ClientPacket *packet = new ClientPacket( cmdList.at(0), this );
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
			return false;
		}
		if( mClientSocket->write( packet->getPacketData() ) < 0 )
		{
			error( QtWarningMsg, "Error during sending client packet", "sendPacket()" );
			return false;
		}
		else
			{ return true; }
	}
	else
	{
		error( QtWarningMsg, "Packet is invalid", "sendPacket()" );
		return false;
	}
}

void ClientConnectionManagerBase::receiveClientData()
{
	if( !checkSocket() )
	{
		error( QtWarningMsg, "Client socket is not ready, checkSocket() failed", "receiveClientData()" );
		return;
	}

	//is it worth to read?
	if( mClientSocket->readBufferSize() < 5 )	//4bytes of packetSize + 1 is the minimum size
		{ return; }

	// read packetSize
	quint16 packetSize = ClientPacket::readPacketSize( mClientSocket->peek(4) );
	if( mClientSocket->readBufferSize() < packetSize+4 )
		{ return; }

	ClientPacket *packet = ClientPacket::fromPacketData( mClientSocket->read( packetSize+4 ) );
	if( !packet )
	{
		error( QtWarningMsg, "Failed to create CLientPacket from data", "receiveClientData()" );
		return;
	}

	if( packet->isValid() )
		{ emit packetReceived( packet ); }
	else
	{
		error( QtWarningMsg, "Received packet is invalid", "receiveClientData()" );
		return;
	}
}

void ClientConnectionManagerBase::handleReceivedPacket(ClientPacket *packet)
{
	const QList<ClientCommandBase*> packetCommands = packet->getCommands();
	for( int i=0; i<packetCommands.size(); ++i )
	{
		if( packetCommands.at(i)->isValid() )
		{ emit commandReceived( packetCommands.at(i) ); }
		else
		{
			error( QtWarningMsg, QString("Received command (%1) is invalid, dropped").arg(packetCommands.at(i)->getName()), "handleReceivedPacket()" );
			/// @todo This is just ugly... This way packet doesn't know that this command was deleted.
			packetCommands.at(i)->deleteLater();
		}
	}
	packet->destroyShell();
}

void ClientConnectionManagerBase::reflex( ClientCommandBase *command )
{
	if( command->getName() == "heartBeat" )
		{ replyHeartBeat( (ClientCommandHeartBeat*)command ); }
	else if( command->getName() == "handShake" )
		{ ackHandShake( (ClientCommandHandShake*)command ); }
}

void ClientConnectionManagerBase::replyHeartBeat( ClientCommandHeartBeat* incomingHeartBeat )
{
	sendCommand( incomingHeartBeat->cloneReply() );
	incomingHeartBeat->deleteLater();
}

void ClientConnectionManagerBase::ackHandShake( ClientCommandHandShake *handShake )
{
	mState = connectionHandShaking;
	/// @todo implement. Two-step? client must ack server hS reply, and just after that will the connection be ready
	//handShake->deleteLater();
	mState = connectionReady;
}

bool ClientConnectionManagerBase::checkSocket()
{
	return ( mClientSocket &&
			mClientSocket->isOpen() &&
			mClientSocket->isReadable() &&
			mClientSocket->isWritable()
			);
}
