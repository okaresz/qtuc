#include "ClientCommandSubscribe.h"

using namespace QtuC;

ClientCommandSubscribe::ClientCommandSubscribe() :
	ClientCommandBase(),
	mFrequency(0)
{
	mName = "subscribe";
	mClass = clientCommandControl;
}

ClientCommandSubscribe::ClientCommandSubscribe( int freq, const QString &hwInterface, const QString &varName ) :
	ClientCommandBase(),
	mFrequency(freq),
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

	mVariable = cmdElement.attribute("var");
	mHwInterface = cmdElement.attribute("hwi");
	bool ok;
	mFrequency = cmdElement.attribute("freq").toInt(&ok);
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
	clone->mFrequency = mFrequency;
	return clone;
}

QDomElement ClientCommandSubscribe::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	cmdElement.setAttribute( "freq", QString::number(mFrequency) );
	cmdElement.setAttribute( "hwi", mHwInterface );
	cmdElement.setAttribute( "var", mVariable );

	return cmdElement;
}

bool ClientCommandSubscribe::isValid() const
{
	return ( mFrequency > 0 ) && ClientCommandBase::isValid();
}
