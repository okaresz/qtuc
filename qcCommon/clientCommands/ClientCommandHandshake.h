#ifndef CLIENTCOMMANDHANDSHAKE_H
#define CLIENTCOMMANDHANDSHAKE_H

#include "ClientCommandBase.h"
#include <QObject>

namespace QtuC
{

/** Handshake command.
  *	Used to intruduce client and server to each other.
  *	@todo implement from attribute*/
class ClientCommandHandshake : public ClientCommandBase
{
	Q_OBJECT
public:
	ClientCommandHandshake();

	/** Create an empty acknowledge handshake.
	  *	To an ACK handshake, info is not needed, as this is used after both ends sent their info handshake.
	  *	@param ack ACK value. True if client is accepted, false if not.*/
	ClientCommandHandshake( bool ack );

	/** Create handshake.
	  *	@param info The self-info to send with handshake. must contain id at least.*/
	ClientCommandHandshake( QHash<QString,QString> &info ) ;

	/** Create acknowledge handshake.
	  *	Create a handshake in reply to another.
	  *	@param info The self-info to send with handshake. must contain id at least.
	  *	@param ack The value of the ack param.*/
	ClientCommandHandshake( QHash<QString,QString> &info, bool ack );


	/// @name Inherited methods from ClientCommandBase.
	/// @{

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid() const;

	/// @}


	/** Return info contained in the handShake.
	  *	@return The handshake info.*/
	const QHash<QString,QString> &getInfo() const
		{ return mInfo; }

	/** Get if this handshake is an ACK handshake (reply to another)
	  *	Info may be empty.
	  *	@return True if this handshake is an ACK, false otherwise.*/
	bool isAck() const
		{ return mIsAck; }

	/** Get the ACK value of this handshake.
	  *	@return The ACK value of this handshake.*/
	bool getAck() const
		{ return mAckVal; }

private:

	/** Set handshake info..
	  *	@param info The self-info to send with handshake. must contain id at least.
	  *	@return True if info is valid and set, false otherwise.*/
	bool setInfo( QHash<QString,QString> &info );

	QHash<QString,QString> mInfo;	///< Details of the handshake
	bool mIsAck;
	bool mAckVal;
};

}	//QtuC::
#endif // CLIENTCOMMANDHANDSHAKE_H
