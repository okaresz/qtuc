#ifndef CLIENTCOMMANDBASE_H
#define CLIENTCOMMANDBASE_H

#include <QDomElement>
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** ClientCommandBase class.
 *	Base class of all client command.*/
class ClientCommandBase : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** ClientCommand class type.
	  * @todo What to do with this?...
	  *	 * <b>undefined</b>: Undefined, invalid command.
	  *  * <b>device</b>: These command contain a deviceCommand, and are used to get/set a state variable or call a device function.
	  *  * <b>control</b>: These commands are designated for the proxy or the client and can be various control commands like quit or heartBeat, etc...*/
	enum commandClass_t
	{
		clientCommandUndefined,
		clientCommandControl,
		clientCommandDevice
	};

	/** Convert commandClass_t to string.
	  *	@param cmdClass The command class to convert.
	  *	@return The string representation of the command class.*/
	static QString commandClassToString( commandClass_t cmdClass );

	/** Convert string to commandClass_t.
	  *	@param cmdClassStr The string representation of the command class.
	  *	@return The command class (commandClass_t).*/
	static commandClass_t commandClassFromString( const QString &cmdClassStr );

	/** Get command class.
	  *	@return Command class.*/
	commandClass_t getClass() const
		{ return mClass; }

	/** Set command class.
	  *	@param cmdClass The new command class.*/
	void setClass( commandClass_t cmdClass )
		{ mClass = cmdClass; }

	/** Create an empty, invalid client command.*/
	ClientCommandBase( QObject *parent = 0 );

	virtual ~ClientCommandBase();

	/** Apply a markup element to a client command.
	  *	The element name should match the name of the command. This function will parse the element and its children and build the command object from them,
	  *	@param cmdElement XML element of the command.
	  *	@return True on success, false otherwise.*/
	virtual bool applyDomElement( const QDomElement &cmdElement ) = 0;

	/** Clone a client command apply the passed markup element to the clone.
	  *	This function will parse the element and its children and build the command object from them,
	  *	A command must be cloned from its own class (A heartBeat command can only be cloned from another heartBeat command).
	  *	The element name should match the name of the command.
	  *	@param cmdElement XML element of the command.
	  *	@return If the clone and apply is successful, returns the new client command object, otherwise 0.*/
	virtual ClientCommandBase *cloneWithDomElement( const QDomElement &cmdElement );

	/** Clone ClientCommand object.
	  *	@warning When implementing this method, do not clone the instance-specific members (like id, or isReplyTo, etc..)! There's exactClone() for that.*/
	virtual ClientCommandBase *clone() = 0;

	/** Clone this command exactly.
	  *	Unlike clone(), exactClone() creates an exact clone of the command instance.
	  *	@return An exact clone.*/
	virtual ClientCommandBase *exactClone() = 0;

	/** Generate the markup element of the command.
	  *	@return The XML markup element of the command.*/
	virtual QDomElement getDomElement() const = 0;

	/** Get if the command is valid.
	  *	Command is valid if it has every necessary parameter with the correct values.
	  *	@return True if command is valid, false otherwise.*/
	virtual bool isValid() const;

	/** Get the name of the command.
	  *	@return The name of the command.*/
	virtual const QString getName() const
		{ return mName; }

	///** @todo slkdjfsdlfjsdlkfjsdlfk*/
	//bool operator==( ClientCommandBase *otherCommand );

protected:

	/** Check if tag name of cmdElement matches the command name.
	  *	@param cmdElement The element to check.
	  *	@return True on match, false otherwise.*/
	bool checkTagName( const QDomElement &cmdElement );

	QString mName;	///< Name of the command. Also tag name in the markup.
	commandClass_t mClass;	///< The class of the command,

};

}	//QtuC::
#endif //CLIENTCOMMANDBASE_H

