#ifndef CLIENTCOMMANDHEARTBEAT_H
#define CLIENTCOMMANDHEARTBEAT_H

#include "ClientCommandBase.h"

namespace QtuC
{

class ClientCommandHeartBeat : public ClientCommandBase
{
	Q_OBJECT
public:
	ClientCommandHeartBeat( QObject *parent = 0 );

	// Inherited methods

	bool applyDomElement( const QDomElement &cmdElement );

	ClientCommandBase *cloneWithDomElement( const QDomElement &cmdElement );

	ClientCommandBase *clone();

	QDomElement getDomElement() const;

	bool isValid() const;
};

}	//QtuC::
#endif // CLIENTCOMMANDHEARTBEAT_H
