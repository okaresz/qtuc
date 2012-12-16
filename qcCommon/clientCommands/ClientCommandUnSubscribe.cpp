#include "ClientCommandUnSubscribe.h"

using namespace QtuC;

ClientCommandUnSubscribe::ClientCommandUnSubscribe( const QString &hwInterface, const QString &varName ) :
	ClientCommandBase(),
	mHwInterface(hwInterface),
	mVariable(varName)
{
	mName = "unSubscribe";
	mClass = clientCommandControl;
}

bool ClientCommandUnSubscribe::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mVariable = cmdElement.attribute("var");
	mHwInterface = cmdElement.attribute("hwi");

	return true;
}

ClientCommandBase *ClientCommandUnSubscribe::clone()
{
	return new ClientCommandUnSubscribe();
}

ClientCommandBase *ClientCommandUnSubscribe::exactClone()
{
	ClientCommandUnSubscribe *clone = new ClientCommandUnSubscribe();
	clone->mVariable = mVariable;
	clone->mHwInterface = mHwInterface;
	return clone;
}

QDomElement ClientCommandUnSubscribe::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	cmdElement.setAttribute( "hwi", mHwInterface );
	cmdElement.setAttribute( "var", mName );

	return cmdElement;
}

bool ClientCommandUnSubscribe::isValid() const
{
	return ClientCommandBase::isValid();
}
