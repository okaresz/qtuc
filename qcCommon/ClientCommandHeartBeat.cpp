#include "ClientCommandHeartBeat.h"

using namespace QtuC;

/// 0 means invalid
quint32 ClientCommandHeartBeat::heartBeatCounter = 1;

ClientCommandHeartBeat::ClientCommandHeartBeat( QObject *parent ) : ClientCommandBase(parent), mIsReply(false)
{
	mName = "heartBeat";
	mClass = clientCommandControl;
	mId = heartBeatCounter++;
}

ClientCommandHeartBeat *ClientCommandHeartBeat::cloneReply()
{
	ClientCommandHeartBeat *replyBeat = (ClientCommandHeartBeat*)clone();
	replyBeat->mAck = getId();
	replyBeat->mIsReply = true;
	return replyBeat;
}

bool ClientCommandHeartBeat::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mAck = cmdElement.attribute("id").toInt();
	mIsReply = true;
	return true;
}

ClientCommandBase *ClientCommandHeartBeat::cloneWithDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return 0; }

	ClientCommandHeartBeat *cmd = (ClientCommandHeartBeat*)clone();
	if( cmd->applyDomElement(cmdElement) )
	{
		error( QtWarningMsg, QString("Failed to apply markup element to %1").arg(mName), "cloneWithDomElement()" );
		return 0;
	}
	else
		{ return cmd; }
}

ClientCommandBase *ClientCommandHeartBeat::clone()
{
	return new ClientCommandHeartBeat(parent());
}

ClientCommandBase *ClientCommandHeartBeat::exactClone()
{
	ClientCommandHeartBeat *clone = new ClientCommandHeartBeat(parent());
	--heartBeatCounter;
	clone->mId = mId;
	clone->mName = mName;
	clone->mAck = mAck;
	clone->mIsReply = mIsReply;
	return clone;
}

QDomElement ClientCommandHeartBeat::getDomElement() const
{
	QDomElement cmdElement;
	cmdElement.setTagName( mName );
	if( mIsReply )
		{ cmdElement.setAttribute( "ack", mAck ); }
	else
		{ cmdElement.setAttribute( "id", mId ); }
	return cmdElement;
}

bool ClientCommandHeartBeat::isValid() const
{
	return !( mIsReply && mAck == 0 );
}
