#ifndef CLIENTCOMMANDSUBSCRIBE_H
#define CLIENTCOMMANDSUBSCRIBE_H

#include "ClientCommandBase.h"

namespace QtuC
{

/** Subscribe to variable(s).
*	With this command the client can subscribe to a variable (or variables).
*	If subscription is accepted, the proxy will send a device get command to the client with the requested frequency.*/
class ClientCommandSubscribe : public ClientCommandBase
{
	Q_OBJECT
public:
	ClientCommandSubscribe();

	/** Create a subscribe command with passed information.
	  *	@param freq Update frequency, Hz. Compulsory.
	  *	@param hwInterface Hardware interface. If omitted, all variable in all interfaces will be fed to the client.
	  *	@param varName Name of the variable to subscribe. If omitted, the client will subscribe to all the variables in the hardware interface.*/
	ClientCommandSubscribe( int freq, const QString &hwInterface = QString(), const QString &varName = QString() );


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

	/** Get the frequency of the subscription.
	  *	@return The frequency of the subscription.*/
	unsigned int getFrequency() const
		{ return mFrequency; }

private:
	unsigned int mFrequency;
	QString mHwInterface;
	QString mVariable;

};

}	//QtuC::
#endif // CLIENTCOMMANDSUBSCRIBE_H
