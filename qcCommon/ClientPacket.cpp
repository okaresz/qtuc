#include "ClientPacket.h"
#include "ConnectionServer.h"
#include <QtEndian>

using namespace QtuC;

ClientPacket::ClientPacket( const QByteArray & packetData, QObject *parent ) : ErrorHandlerBase(parent)
{
	mIdNum = mPacketCount++;
}

ClientPacket::ClientPacket(QObject *parent)  : ErrorHandlerBase(parent)
{
	mIdNum = mPacketCount++;
}

ClientPacket::ClientPacket( ClientCommandBase * clientCommand, QObject *parent ) : ErrorHandlerBase(parent)
{
	mIdNum = mPacketCount++;
}

int ClientPacket::getIDNumber() const
{
	return mIdNum;
}

const QString ClientPacket::getReplyTo() const
{
	return mReplyTo;
}

packetClass ClientPacket::getClass() const
{
	return mClass;
}

void ClientPacket::setReplyTo( const QString &replyToID )
{
	mReplyTo = replyToID;
}

bool ClientPacket::setClass( packetClass_t pClass )
{
	if( mCmdList.empty() )
	{
		mClass = pClass;
		return true;
	}
	else
	{
		error( QtWarningMsg, "Try to set packet class after commands have been appended, ignore.", "setClass()" );
		return false;
	}
}

bool ClientPacket::append( ClientCommandBase *clientCommand )
{
	if( mClass == packetUndefined )
	{
		error( QtWarningMsg, "Try to append command, but packet class is undefined, ignore command", "append()" );
		return false;
	}
	else
	{
		mCmdList.append(clientCommand);
		return true;
	}
}

QDomDocument *ClientPacket::buildMarkup()
{
	QDomDocument *packetMarkup = new QDomDocument();
	QDomElement packetElement = packetMarkup->createElement("packet");
	packetElement.setAttribute( "id", QString( ConnectionServer::serverId + "#" + QString::number(mIdNum) ) );
	if( !mReplyTo.isEmpty() )
		{ packetElement.setAttribute( "re", mReplyTo ); }

	/// @todo append client commands

	packetMarkup->appendChild(packetElement);
	return packetMarkup;
}

const QByteArray ClientPacket::getPacketData() const
{
	QDomDocument *packetMarkup = buildMarkup();
	if( !packetMarkup )
	{
		error( QtWarningMsg, "Failed to build packet markup", "getPacketData()" );
		return QByteArray();
	}
	else
	{
		QByteArray packetData = packetMarkup->toByteArray(-1);

		quint16 packetSize = packetData.size() + 1;	// ha! the null terminator!
		uchar packetSizeCharBigEndian[4];
		qToBigEndian( packetSize, packetSizeCharBigEndian );

		QByteArray rawPacket( packetSizeCharBigEndian, 4 );
		rawPacket.append( packetData );
		return rawPacket;
	}
}

ClientPacket* ClientPacket::fromPacketData( const QByteArray &rawPacket )
{
	qint16 packetSize = readPacketSize( rawPacket );
	if( rawPacket.size() < packetSize + 4 )
	{
		error( QtWarningMsg, "Failed to create ClientPacket: data too short", "fromPacketData()" );
		return 0;
	}
	else
	{
		if( rawPacket.size() > packetSize + 4 )
			{ debug( debugLevelVerbose, "Data length is more than packet size suggests! Bad things will happen?...", "fromPacketData()" ); }

		QByteArray packetData( rawPacket.right( packetSize-1 ) );	// don't count null terminator
		return new ClientPacket( packetData );
	}
}

qint16 ClientPacket::readPacketSize( const QByteArray &rawData ) const
{
	if( rawData.size() < 4 )
	{
		error( QtWarningMsg, "Unable to read packet size: data too short", "readPacketSize()" );
		return 0;
	}
	else
	{
		return qFromBigEndian( rawData.left(4).data() );
	}
}

