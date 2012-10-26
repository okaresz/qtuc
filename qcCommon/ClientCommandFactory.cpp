#include "ClientCommandFactory.h"

#include "ClientCommandHeartBeat.h"

using namespace QtuC;

ClientCommandFactory::ClientCommandFactory( QObject *parent ) : ErrorHandlerBase(parent)
{
	registerClientCommand( new ClientCommandHeartBeat() );
}

bool ClientCommandFactory::registerClientCommand( ClientCommandBase *cmdPrototype )
{
	mCommandPrototypes.append( cmdPrototype );
}

ClientCommandBase *ClientCommandFactory::buildCommand( const QDomElement &cmdElement )
{
	for( int i=0; i<mCommandPrototypes.size(); ++i )
	{
		if( mCommandPrototypes.at(i)->getName() == cmdElement.tagName() )
		{
			return mCommandPrototypes.at(i)->cloneWithDomElement( cmdElement );
		}
	}

	error( QtWarningMsg, QString("No matching ClientCommandPrototype found for commandElement: tagname: %1").arg(cmdElement.tagName()), "buildCommand()" );
	return 0;
}
