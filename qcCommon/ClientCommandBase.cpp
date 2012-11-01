#include "ClientCommandBase.h"

using namespace QtuC;

ClientCommandBase::ClientCommandBase( QObject *parent ) : ErrorHandlerBase(parent), mClass(clientCommandUndefined)
{}

ClientCommandBase::~ClientCommandBase()
{
	/// @todo implement?
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
