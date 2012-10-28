#ifndef CLIENTCOMMANDFACTORY_H
#define CLIENTCOMMANDFACTORY_H

#include "ErrorHandlerBase.h"
#include "ClientCommandBase.h"
#include <QList>
#include <QDomElement>

// ClientCommand class headers.
#include "ClientCommandHeartBeat.h"

namespace QtuC
{

/** Factory class to manage the different types of client commands,*/
class ClientCommandFactory : public ErrorHandlerBase
{
	Q_OBJECT
public:
	/** C'tor*/
	ClientCommandFactory( QObject *parent = 0 );

	~ClientCommandFactory();

	/** Register a new client command type.
	  *	This static fuction can be called from anywhere in the code, however the recommended spot is the constructor of ClientCommandFactory.
	  *	If you implement a new client command (derived from ClientCommandBase), you must register an istance with this function.
	  *	After that, ClientCommandFactory will recognize and clone it if that command arrives.
	  *	@param cmdPrototype A prototype instance of the command.
	  *	@return True if the command is registered, false on failure.*/
	static bool registerClientCommand( ClientCommandBase *cmdPrototype );

	/** Get the stored command prototype with the name cmdName.
	  *	@warning Use this function with care, as this returns a pointer to the stored protoype object, which may be used elsewhere.
	  *	When in doubt, make a copy with cloneCommand().
	  *	@param cmdName The name of the command to return.
	  *	@return The stored prototype of the client command.*/
	const ClientCommandBase *getCommand( const QString &cmdName );

	/** Get command prototype with the passed commandElement applied to it.
	 *	Read the tagName of the cmdElement, get the stored prototype of the command, and set its parameters based on the element.</br>
	 *	@warning Use this function with care, as this modifies the stored prototype, so while it's a fast method to handle a command, only one can be handled at a time.
	 *	If you would like to use a command object for a longer time, or pass to a function/class that who knows what will do with it, use cloneCommand().
	 *	@param cmdElement The markup representation of the command.
	 *	@return The stored prototype of the client command with the passed command representation applied to it.*/
	const ClientCommandBase *getCommand( const QDomElement &cmdElement );

	/** Clone the command prototype andapply commandElement to it.
	 *	Read the tagName of the cmdElement, clone the stored prototype of the command, and set its parameters based on the element.</br>
	 *	@param cmdElement The markup representation of the command.
	 *	@return A clone of th stored command prototype with the passed command representation applied to it.*/
	ClientCommandBase *cloneCommand( const QDomElement &cmdElement );

private:
	static QList<ClientCommandBase*> mCommandPrototypes;	///< List of registered client command prototypes.
};

}	//QtuC::
#endif // CLIENTCOMMANDFACTORY_H