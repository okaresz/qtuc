#include "ClientCommandFactory.h"

using namespace QtuC;

QList<ClientCommandBase*> ClientCommandFactory::mCommandPrototypes = QList<ClientCommandBase*>();

ClientCommandFactory::ClientCommandFactory( QObject *parent ) : ErrorHandlerBase(parent)
{
	registerCommand( new ClientCommandHeartBeat() );
	registerCommand( new ClientCommandHandshake() );
	registerCommand( new ClientCommandDeviceApi() );
	registerCommand( new ClientCommandReqDeviceApi() );
	registerCommand( new ClientCommandSubscribe() );
	registerCommand( new ClientCommandUnSubscribe() );
	registerCommand( new ClientCommandReqDeviceInfo() );
	registerCommand( new ClientCommandDeviceInfo() );

	// Hah! How tricky I am! The deviceCommands with one class.
	registerCommand( new ClientCommandDevice(deviceCmdGet) );
	registerCommand( new ClientCommandDevice(deviceCmdSet) );
	registerCommand( new ClientCommandDevice(deviceCmdCall) );
}

ClientCommandFactory::~ClientCommandFactory()
{
	for( int i=0; i<mCommandPrototypes.size(); ++i )
	{
		if( mCommandPrototypes.at(i) )
		{
			delete mCommandPrototypes.value(i);
			mCommandPrototypes.removeAt(i);
		}
	}
}

bool ClientCommandFactory::registerCommand( ClientCommandBase *cmdPrototype )
{
	if( cmdPrototype )
	{
		mCommandPrototypes.append( cmdPrototype );
		return true;
	}
	else
		{ return false; }
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
