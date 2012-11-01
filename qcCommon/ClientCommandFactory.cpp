#include "ClientCommandFactory.h"

using namespace QtuC;

QList<ClientCommandBase*> ClientCommandFactory::mCommandPrototypes = QList<ClientCommandBase*>();

ClientCommandFactory::ClientCommandFactory( QObject *parent ) : ErrorHandlerBase(parent)
{
	registerCommand( new ClientCommandHeartBeat(this) );
}

ClientCommandFactory::~ClientCommandFactory()
{

}

bool ClientCommandFactory::registerCommand( ClientCommandBase *cmdPrototype )
{
	mCommandPrototypes.append( cmdPrototype );
}

const ClientCommandBase *ClientCommandFactory::getCommand(const QString &cmdName)
{
	for( int i=0; i<mCommandPrototypes.size(); ++i )
	{
		if( mCommandPrototypes.at(i)->getName() == cmdName )
			{ return mCommandPrototypes.at(i); }
	}

	error( QtWarningMsg, QString("No matching ClientCommandPrototype found wih name \"%1\"").arg(cmdName), "getCommand()" );
	return 0;
}

const ClientCommandBase *ClientCommandFactory::getCommand(const QDomElement &cmdElement)
{
	for( int i=0; i<mCommandPrototypes.size(); ++i )
	{
		if( mCommandPrototypes.at(i)->getName() == cmdElement.tagName() )
		{
			if( mCommandPrototypes.at(i)->applyDomElement( cmdElement ) )
				{ return mCommandPrototypes.at(i); }
			else
			{
				errorDetails_t errDet;
				errDet.insert( "element_tagName", cmdElement.tagName() );
				errDet.insert( "commandName", mCommandPrototypes.at(i)->getName() );
				error( QtWarningMsg, "Failed to apply markup element to command object", "getCommand()", errDet );
				return 0;
			}
		}
	}

	error( QtWarningMsg, QString("No matching ClientCommandPrototype found for commandElement: tagname: %1").arg(cmdElement.tagName()), "buildCommand()" );
	return 0;
}

ClientCommandBase *ClientCommandFactory::cloneCommand( const QDomElement &cmdElement )
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
