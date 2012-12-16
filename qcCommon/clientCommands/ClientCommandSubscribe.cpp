#include "ClientCommandSubscribe.h"

using namespace QtuC;

ClientCommandSubscribe::ClientCommandSubscribe() :
	ClientCommandBase(),
	mInterval(0)
{
	mName = "subscribe";
	mClass = clientCommandControl;
}

ClientCommandSubscribe::ClientCommandSubscribe( quint32 interval, const QString &hwInterface, const QString &varName ) :
	ClientCommandBase(),
	mInterval(interval),
	mHwInterface(hwInterface),
	mVariable(varName)
{
	mName = "subscribe";
	mClass = clientCommandControl;
}

bool ClientCommandSubscribe::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mVariable = cmdElement.attribute("variable");
	mHwInterface = cmdElement.attribute("hwInterface");
	bool ok;
	mInterval = cmdElement.attribute("interval").toInt(&ok);
	if( !ok )
	{
		errorDetails_t errDet;
		errDet.insert( "varName", mVariable );
		errDet.insert( "hwiName", mHwInterface );
		errDet.insert( "freqStr", cmdElement.attribute("freq") );
		error( QtWarningMsg, "Invalid string for frequency in subscribe command", "applyDomElement()" );
		return false;
	}
	return true;
}

ClientCommandBase *ClientCommandSubscribe::clone()
{
	return new ClientCommandSubscribe();
}

ClientCommandBase *ClientCommandSubscribe::exactClone()
{
	ClientCommandSubscribe *clone = new ClientCommandSubscribe();
	clone->mVariable = mVariable;
	clone->mHwInterface = mHwInterface;
	clone->mInterval = mInterval;
	return clone;
}

QDomElement ClientCommandSubscribe::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	cmdElement.setAttribute( "interval", QString::number(mInterval) );
	cmdElement.setAttribute( "hwInterface", mHwInterface );
	cmdElement.setAttribute( "variable", mVariable );

	return cmdElement;
}

bool ClientCommandSubscribe::isValid() const
{
	return ( mInterval > 0 ) && ClientCommandBase::isValid();
}
