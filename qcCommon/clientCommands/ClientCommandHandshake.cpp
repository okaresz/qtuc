#include "ClientCommandHandshake.h"

using namespace QtuC;

ClientCommandHandshake::ClientCommandHandshake() :
	ClientCommandBase()
{
	mName = "handshake";
	mClass = clientCommandControl;
}

ClientCommandHandshake::ClientCommandHandshake(bool ack) :
	ClientCommandBase(),
	mIsAck(true),
	mAckVal(ack)
{
	mName = "handshake";
	mClass = clientCommandControl;
}

ClientCommandHandshake::ClientCommandHandshake(QHash<QString, QString> &info) :
	ClientCommandBase(),
	mIsAck(false)
{
	mName = "handshake";
	mClass = clientCommandControl;
	setInfo(info);
}

ClientCommandHandshake::ClientCommandHandshake(QHash<QString, QString> &info, bool ack) :
	ClientCommandBase(),
	mIsAck(true),
	mAckVal(ack)
{
	mName = "handshake";
	mClass = clientCommandControl;
	setInfo(info);
}

bool ClientCommandHandshake::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mIsAck = cmdElement.hasAttribute("ack");
	mAckVal = mIsAck && ( cmdElement.attribute("ack", "false") == "true" );
	QDomElement infoElement = cmdElement.firstChildElement();
	while( !infoElement.isNull() )
	{
		mInfo.insert( infoElement.tagName(), infoElement.text() );
		infoElement = infoElement.nextSiblingElement();
	}
	return true;
}

ClientCommandBase *ClientCommandHandshake::clone()
{
	return new ClientCommandHandshake();
}

ClientCommandBase *ClientCommandHandshake::exactClone()
{
	ClientCommandHandshake *clone = new ClientCommandHandshake();
	clone->mAckVal = mAckVal;
	clone->mIsAck = mIsAck;
	clone->mInfo = mInfo;
	return clone;
}

QDomElement ClientCommandHandshake::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	if( mIsAck )
	{
		if( mAckVal )
			{ cmdElement.setAttribute( "ack", "true" ); }
		else
			{ cmdElement.setAttribute( "ack", "false" ); }
	}

	QHash<QString,QString>::const_iterator infoItem = mInfo.constBegin();
	while( infoItem != mInfo.constEnd() )
	{
		QDomElement infoElement = dom.createElement( infoItem.key() );
		infoElement.appendChild( dom.createTextNode( infoItem.value() ) );
		cmdElement.appendChild( infoElement );
		++infoItem;
	}

	return cmdElement;
}

bool ClientCommandHandshake::isValid() const
{
	return ( ( mInfo.contains("id") || mIsAck ) && ClientCommandBase::isValid() );
}

bool ClientCommandHandshake::setInfo(QHash<QString, QString> &info)
{
	if( !info.contains("id") )
	{
		//error( QtWarningMsg, "incomplete info was supplied to handshake command, can not create handshake", "ClientCommandHandshake()" );
		return false;
	}
	else
	{
		mInfo = info;
		return true;
	}
}
