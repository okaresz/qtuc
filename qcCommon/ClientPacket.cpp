#include "ClientPacket.h"
#include <QtEndian>
#include "ClientCommandBase.h"
#include <QDomDocument>

using namespace QtuC;

ClientCommandFactory *ClientPacket::mCommandFactoryPtr = 0;
QString ClientPacket::mSelfId = QString("qcProxy");
quint64 ClientPacket::mPacketCount = 0;

ClientPacket::ClientPacket( const QDomElement &packetElement, QObject *parent ) : ErrorHandlerBase(parent)//, mClass(packetUndefined)
{
	mIdNum = ++mPacketCount;

	if( mCommandFactoryPtr == 0 )
	{
		error( QtWarningMsg, "mCommandFactoryPtr is null, can't create ClientPacket, you must set it before creating a packet", "ClientPacket()" );
		return;
	}

	//mClass = (packetClass_t)metaObject()->enumerator( metaObject()->indexOfEnumerator("packetClass_t") ).keyToValue( packetElement.attribute("class").toStdString().c_str() );

	QDomElement commandElement = packetElement.firstChildElement();
	while( !commandElement.isNull() )
	{
		ClientCommandBase *cmd = mCommandFactoryPtr->cloneCommand(commandElement);
		if( !cmd )
			{ error( QtWarningMsg, "Invalid clientCommand, dropped.", "ClientPacket()" ); }
		else
		{
			//if( cmd->getPacketClass() != mClass )
				//{ error( QtWarningMsg, QString("Command (%1) packet class (%2) differs from packet class (%3)! Command dropped.").arg(cmd->getName(),cmd->getPacketClass(),mClass), "ClientPacket()" ); }
			//else
				{ mCmdList.append( cmd ); }
		}
		commandElement = commandElement.nextSiblingElement();
	}
}

ClientPacket::ClientPacket()  : ErrorHandlerBase(0)//, mClass(packetUndefined)
{
	mIdNum = ++mPacketCount;
	if( mCommandFactoryPtr == 0 )
	{
		error( QtWarningMsg, "mCommandFactoryPtr is null, can't create ClientPacket, you must set it before creating a packet", "ClientPacket()" );
		return;
	}
}

ClientPacket::ClientPacket( ClientCommandBase *clientCommand, QObject *parent ) : ErrorHandlerBase(parent)//, mClass(packetUndefined)
{
	mIdNum = ++mPacketCount;

	if( mCommandFactoryPtr == 0 )
	{
		error( QtWarningMsg, "mCommandFactoryPtr is null, can't create ClientPacket, you must set it before creating a packet", "ClientPacket()" );
		return;
	}

	if( !clientCommand->isValid() )
		{ return; }

	//mClass = clientCommand->getPacketClass();
	mCmdList.append( clientCommand );
}

ClientPacket::~ClientPacket()
{
	for( int i=0; i<mCmdList.size(); ++i )
	{
		// Commands may be destroyed from outside of this object
		/// @todo What behaviour is this!?...
		if( mCmdList.at(i) )
			{ delete mCmdList.value(i); }
	}
	//debug( debugLevelVeryVerbose, "Destroyed", "~ClientPacket()" );
}

bool ClientPacket::isValid()
{
	if(
	   mCmdList.isEmpty() ||
	   mIdNum == 0
	  // mClass == packetUndefined
	  )
	{ return false; }
	return true;
}

quint64 ClientPacket::getIDNumber() const
{
	return mIdNum;
}

const QString ClientPacket::getID() const
{
	return QString( mSelfId + "#" + QString::number(mIdNum) );
}

const QString ClientPacket::getReplyTo() const
{
	return mReplyTo;
}

//ClientPacket::packetClass_t ClientPacket::getClass() const
//{
//	return mClass;
//}

const QList<ClientCommandBase *> ClientPacket::getCommands()
{
	return mCmdList;
}

void ClientPacket::setReplyTo( const QString &replyToID )
{
	mReplyTo = replyToID;
}

void ClientPacket::setReplyTo(const ClientPacket &replyToPacket)
{
	mReplyTo = replyToPacket.getID();
}

/*bool ClientPacket::setClass( packetClass_t pClass )
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
}*/

bool ClientPacket::appendCommand( ClientCommandBase *clientCommand )
{
//	if( mClass == packetUndefined )
//	{
//		error( QtWarningMsg, "Try to append command, but packet class is undefined, ignore command", "append()" );
//		return false;
//	}
//	else
//	{
		if( !clientCommand->isValid() )
		{
			error( QtWarningMsg, QString("Failed to append client command (%1): command is invalid").arg(clientCommand->getName()), "appendCommand()" );
			return false;
		}
//		if( clientCommand->getPacketClass() != mClass )
//		{
//			error( QtWarningMsg, "Failed to append client command: packet class mismatch", "appendCommand()" );
//			return false;
//		}
		mCmdList.append(clientCommand);
		return true;
		//	}
}

ClientCommandBase *ClientPacket::detachCommand(int index)
{
	return removeCommand(index);
}

ClientCommandBase *ClientPacket::detachCommand(ClientCommandBase *command)
{
	for( int i=0; i<mCmdList.size(); ++i )
	{
		if( mCmdList.at(i) == command )
		{
			return removeCommand(i);
		}
	}
	return 0;
}

void ClientPacket::destroyShell()
{
	mCmdList.clear();
	deleteLater();
}

QDomDocument *ClientPacket::buildMarkup() const
{
	QDomDocument *packetMarkup = new QDomDocument();
	QDomElement packetElement = packetMarkup->createElement("packet");
	packetElement.setAttribute( "id", getID() );
	/// @todo packetClass
	//packetElement.setAttribute( "class", QString( metaObject()->enumerator( metaObject()->indexOfEnumerator("packetClass_t") ).valueToKey(mClass) ); );
	if( !mReplyTo.isEmpty() )
		{ packetElement.setAttribute( "re", mReplyTo ); }

	for( int i=0; i<mCmdList.size(); ++i )
		{ packetElement.appendChild( mCmdList.at(i)->getDomElement() ); }

	packetMarkup->appendChild(packetElement);
	return packetMarkup;
}

ClientCommandBase *ClientPacket::removeCommand(int index)
{
	ClientCommandBase *tmp = mCmdList.value(index);
	mCmdList[index] = 0;
	return tmp;
}

const QByteArray ClientPacket::getPacketData() const
{
	QDomDocument *packetMarkup = buildMarkup();
	if( !packetMarkup )
	{
		error( QtWarningMsg, "Failed to build packet markup", "getPacketData()", "ClientPacket" );
		return QByteArray();
	}
	else
	{
		QByteArray packetData = packetMarkup->toByteArray(-1);
		delete packetMarkup;

		quint16 packetSize = packetData.size();
		uchar packetSizeCharBigEndian[sizeof(quint16)];
		qToBigEndian( packetSize, packetSizeCharBigEndian );

		QByteArray rawPacket = QByteArray::fromRawData( (const char*)packetSizeCharBigEndian, sizeof(quint16) );
		rawPacket.append( packetData );
		return rawPacket;
	}
}

ClientPacket* ClientPacket::fromPacketData( const QByteArray &rawPacket )
{
	quint16 packetSize = readPacketSize( rawPacket );
	if( rawPacket.size() < packetSize + sizeof(quint16) )
	{
		error( QtWarningMsg, "Failed to create ClientPacket: data too short", "fromPacketData()", "ClientPacket" );
		return 0;
	}

	if( rawPacket.size() > packetSize + sizeof(quint16) )
		{ debug( debugLevelVerbose, "Data length is more than packet size suggests! Bad things will happen?...", "fromPacketData()", "ClientPacket" ); }

	QByteArray packetData( rawPacket.right( packetSize ) );

	// Let's parse the data!
	QString packetMarkupText = QString::fromUtf8( packetData.data() );
	QDomDocument packetDom;
	QString errMsg;
	int errLine;
	if( !packetDom.setContent( packetMarkupText, false, &errMsg, &errLine ) )
	{
		errorDetails_t errDet;
		errDet.insert( "errMsg", errMsg );
		errDet.insert( "errLine", QString::number(errLine) );
		errDet.insert( "packetData", packetMarkupText );
		error( QtWarningMsg, "Error while parsing packetData", "ClientPacket()", "ClientPacket", errDet );
		return 0;
	}

	QDomElement packetElement = packetDom.documentElement();
	if( !packetElement.hasAttribute("id") )
	{
		errorDetails_t errDet;
		errDet.insert( "packetMarkup", packetMarkupText );
		error( QtWarningMsg, "Packet has no id", "fromPacketData()", "ClientPacket", errDet );
		return 0;
	}

	/*if( !packetElement.hasAttribute("class") )
	{
		errorDetails_t errDet;
		errDet.insert( "packetMarkup", packetMarkupText );
		error( QtWarningMsg, "Packet has no class attribute", "fromPacketData()", errDet );
		return 0;
	}*/

	return new ClientPacket( packetElement );
}

quint16 ClientPacket::readPacketSize( const QByteArray &rawData )
{
	if( rawData.size() < sizeof(quint16) )
	{
		error( QtWarningMsg, "Unable to read packet size: data too short", "readPacketSize()", "ClientPacket" );
		return 0;
	}
	else
	{
		quint16 si = qFromBigEndian<quint16>( (const uchar*)(rawData.left(sizeof(quint16)).data()) );
		return si;
	}
}

bool ClientPacket::setSelfId(const QString &newId)
{
	if( !newId.isEmpty() )
	{
		mSelfId = newId;
		return true;
	}
	else
	{
		error( QtWarningMsg, "Failed to set selfId: empty string", "ClientPacket::setSelfId()", "ClientPacket" );
		return false;
	}
}

