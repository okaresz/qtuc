#include "ClientCommandBase.h"

using namespace QtuC;

ClientCommandBase::ClientCommandBase( QObject *parent ) :
	ErrorHandlerBase(parent),
	mClass(clientCommandUndefined)
{}

ClientCommandBase::~ClientCommandBase()
{}

ClientCommandBase *ClientCommandBase::cloneWithDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return 0; }

	ClientCommandBase *cmd = clone();
	if( !cmd->applyDomElement(cmdElement) )
	{
		error( QtWarningMsg, QString("Failed to apply markup element to %1").arg(mName), "cloneWithDomElement()" );
		return 0;
	}
	else
		{ return cmd; }
}

bool ClientCommandBase::isValid() const
{
	if(
	   mName.isEmpty() ||
	   mClass == clientCommandUndefined
	  )
	{ return false; }
	return true;
}

//bool ClientCommandBase::operator ==(ClientCommandBase *otherCommand)
//{
//	return ( otherCommand->getName() == getName() );
//}

bool ClientCommandBase::checkTagName(const QDomElement &cmdElement)
{
	if( cmdElement.tagName() == mName )
		{ return true; }
	else
	{
		error( QtWarningMsg, QString("Command element tagName (%1) and command name (%2) does not match").arg( cmdElement.tagName(), mName ), "checkTagName()" );
		return false;
	}
}
