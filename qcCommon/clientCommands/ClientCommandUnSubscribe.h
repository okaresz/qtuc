#ifndef CLIENTCOMMANDUNSUBSCRIBE_H
#define CLIENTCOMMANDUNSUBSCRIBE_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** Unsubscribe from variable(s).
*	With this command the client can unsubscribe from a variable (or variables).
*	If the unsubscribe request is accepted, the proxy will stop to send device get commands to the client.*/
class ClientCommandUnSubscribe : public ClientCommandBase
{
	Q_OBJECT
public:

	/** Create a subscribe command with passed information.
	  *	@param hwInterface Hardware interface. If omitted, all variable in all interfaces will be unsibscribed from the client.
	  *	@param varName Name of the variable to unsubscribe. If omitted, the client will unsubscribe from all the variables in the hardware interface.*/
	ClientCommandUnSubscribe( const QString &hwInterface = QString(), const QString &varName = QString() );


	/// @name Inherited methods from ClientCommandBase.
	/// @{

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	ClientCommandBase *exactClone();

	QDomElement getDomElement() const;

	bool isValid() const;

	/// @}


	/** Get subscritpion variable.
	  *	 @return The variable.*/
	const QString getVariable() const
		{ return mVariable; }

	/** Get subscritpion hardware interface.
	  *	 @return The hardware interface.*/
	const QString getHwInterface() const
		{ return mHwInterface; }

private:
	QString mHwInterface;
	QString mVariable;

};

}	//QtuC::
#endif // CLIENTCOMMANDUNSUBSCRIBE_H
