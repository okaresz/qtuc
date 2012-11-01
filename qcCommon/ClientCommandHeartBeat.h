#ifndef CLIENTCOMMANDHEARTBEAT_H
#define CLIENTCOMMANDHEARTBEAT_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** Heartbeat command.
  *	HeartBeats help monitoring connection stability.*/
class ClientCommandHeartBeat : public ClientCommandBase
{
	Q_OBJECT
public:
	ClientCommandHeartBeat( QObject *parent = 0 );

	ClientCommandHeartBeat *cloneReply(  );

	// Inherited methods

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *cloneWithDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid() const;

	/** Get heartBeat ID.
	  *	@return The heartBeat id.*/
	quint16 getId() const
		{ return mId; }

private:
	static quint32 heartBeatCounter;	///< The count heartBeat instantiations.
	quint32 mId;	///< The Id of the heartBeat, set to heartBeatCounter on instantiation..
	quint32 mAck;	///< Store the ID of the heartBeat this instance is a reply to.
	bool mIsReply;	///< Whether this heartBeat instance is a reply to another,
};

}	//QtuC::
#endif // CLIENTCOMMANDHEARTBEAT_H
