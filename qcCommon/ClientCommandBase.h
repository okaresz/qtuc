#ifndef CLIENTCOMMANDBASE_H
#define CLIENTCOMMANDBASE_H

#include <QDomElement>
#include "ClientPacket.h"
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** ClientCommandBase class.
 *	Base class of all client command.*/
class ClientCommandBase : ErrorHandlerBase
{
	Q_OBJECT
public:
	ClientCommandBase( QObject *parent = 0 );

	virtual ~ClientCommandBase();

	/** Create client command from it's markup .
	  *	@param fromElement XML element of the command.
	  *	@return The new client command object or 0 on failure.*/
	virtual bool applyDomElement( const QDomElement &cmdElement ) = 0;

	virtual ClientCommandBase *cloneWithDomElement( const QDomElement &cmdElement ) = 0;

	/** Clone Clientcommand object.
	  *	@warning When implementing this method, do not clone the instance-specific members (like id, or isReplyTo, etc..)! There's exactClone() for that.*/
	virtual ClientCommandBase *clone() = 0;

	/** Clone this command exactly.
	  *	Unlike clone(), exactClone() creates an exact clone of the command instance.
	  *	@return An exact clone.*/
	virtual ClientCommandBase *exactClone() = 0;

	/** Get XML element of the command.
	  *	@return The XML element of the command.*/
	virtual QDomElement getDomElement() const = 0;

	/** Get if the command is valid.
	  *	Command is valid if it has every necessary parameter with the correct values.
	  *	@return True if command is valid, false otherwise.*/
	virtual bool isValid() const;

	/** Get packet class.
	  *	Commands will and can only be wrapped in a packet with the same class.
	  *	@return The packet class for the command.*/
	packetClass_t getPacketClass() const
		{ return mPacketClass; }

	virtual const QString getName() const
		{ return mName; }

protected:

	/** Check if tag name of cmdElement matches the command name.
	  *	@param cmdElement The element to check.
	  *	@return True on match, false otherwise.*/
	bool checkTagName( const QDomElement &cmdElement );

	QString mName;	///< Name of the command. Also tag name in the markup.
	ClientPacket::packetClass_t mPacketClass;	///< The packet class for the command,

};

}	//QtuC::
#endif //CLIENTCOMMANDBASE_H

