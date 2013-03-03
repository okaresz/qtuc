#include "ClientCommandDeviceInfo.h"
#include <QDateTime>

using namespace QtuC;

ClientCommandDeviceInfo::ClientCommandDeviceInfo() :
	ClientCommandBase(),
	mStartupTime(0),
	mStartup(false),
	mPositiveAck(false)
{
	mName = "deviceInfo";
	mClass = clientCommandControl;
}

bool ClientCommandDeviceInfo::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mStartup = cmdElement.attribute("startup") == "true" || cmdElement.attribute("startup") == "1";
	bool ok;
	mStartupTime = cmdElement.attribute("startupTime", "0").toLongLong( &ok, 16 );
	if( !ok )
	{
		error( QtWarningMsg, "Invalid startup timestamp received", "applyDomElement()" );
		//fallback
		if( mStartup )
			{ mStartupTime = QDateTime::currentMSecsSinceEpoch(); }
		else
			{ mStartupTime = 0; }
	}

	QDomElement infoElement = cmdElement.firstChildElement( "info" );
	while( !infoElement.isNull() )
	{
		if( infoElement.attribute("name").isEmpty() )
			{ error( QtWarningMsg, QString("device info name attribute is empty (value: %1)").arg(infoElement.text()), "applyDomElement()" ); }
		else
		{
			if( infoElement.attribute("name") == "positiveAck" )
				{ mPositiveAck = infoElement.text() == "true" || infoElement.text() == "1"; }
			else
				{ mInfoList.insert( infoElement.attribute("name"), infoElement.text() ); }
		}
		infoElement = infoElement.nextSiblingElement( "info" );
	}

	return true;
}

ClientCommandBase *ClientCommandDeviceInfo::clone()
{
	return new ClientCommandDeviceInfo();
}

ClientCommandBase *ClientCommandDeviceInfo::exactClone()
{
	ClientCommandDeviceInfo *clone = new ClientCommandDeviceInfo();
	clone->mStartup = mStartup;
	clone->mStartupTime = mStartupTime;
	clone->mPositiveAck = mPositiveAck;
	clone->mInfoList = mInfoList;
	return clone;
}

QDomElement ClientCommandDeviceInfo::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);
	QString startupStr = mStartup?"true":"false";
	cmdElement.setAttribute( "startup", startupStr );
	cmdElement.setAttribute( "startupTime", QString::number( mStartupTime, 16 ) );

	QDomElement posAckInfoElement = dom.createElement("info");
	posAckInfoElement.setAttribute( "name", "positiveAck" );
	QString posAckStr = mPositiveAck?"true":"false";
	posAckInfoElement.appendChild( dom.createTextNode(posAckStr) );
	cmdElement.appendChild( posAckInfoElement );

	QHash<QString,QString>::const_iterator i = mInfoList.constBegin();
	while( i != mInfoList.constEnd() )
	{
		QDomElement infoElement = dom.createElement("info");
		infoElement.setAttribute( "name", i.key() );
		infoElement.appendChild( dom.createTextNode(i.value()) );
		cmdElement.appendChild( infoElement );
		++i;
	}

	return cmdElement;
}
