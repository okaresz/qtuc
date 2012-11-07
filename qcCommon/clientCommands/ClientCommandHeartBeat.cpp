#include "ClientCommandHeartBeat.h"

using namespace QtuC;

/// 0 means invalid
quint32 ClientCommandHeartBeat::heartBeatCounter = 1;

ClientCommandHeartBeat::ClientCommandHeartBeat() :
	ClientCommandBase(),
	mIsReply(false)
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

ClientCommandBase *ClientCommandHeartBeat::clone()
{
	return new ClientCommandHeartBeat();
}

ClientCommandBase *ClientCommandHeartBeat::exactClone()
{
	ClientCommandHeartBeat *clone = new ClientCommandHeartBeat();
	--heartBeatCounter;
	clone->mId = mId;
	clone->mName = mName;
	clone->mAck = mAck;
	clone->mIsReply = mIsReply;
	return clone;
}

QDomElement ClientCommandHeartBeat::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	if( mIsReply )
		{ cmdElement.setAttribute( "ack", mAck ); }
	else
		{ cmdElement.setAttribute( "id", mId ); }
	return cmdElement;
}

bool ClientCommandHeartBeat::isValid() const
{
	return !( mIsReply && mAck == 0 ) && ClientCommandBase::isValid();
}
