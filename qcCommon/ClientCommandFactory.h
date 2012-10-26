#ifndef CLIENTCOMMANDFACTORY_H
#define CLIENTCOMMANDFACTORY_H

#include "ErrorHandlerBase.h"
#include "ClientCommandBase.h"
#include <QList>
#include <QDomElement>

namespace QtuC
{

class ClientCommandFactory : public ErrorHandlerBase
{
	Q_OBJECT
public:
	ClientCommandFactory( QObject *parent = 0 );

	static bool registerClientCommand( ClientCommandBase *cmdPrototype );

	ClientCommandBase *buildCommand( const QDomElement &cmdElement );

private:
	static QList<ClientCommandBase*> mCommandPrototypes;
};

}	//QtuC::
#endif // CLIENTCOMMANDFACTORY_H
