#include "ClientCommandDeviceApi.h"
#include <QCryptographicHash>

using namespace QtuC;

ClientCommandDeviceApi::ClientCommandDeviceApi( const QString &deviceApiString )
{
	mName = "deviceAPI";
	mClass = clientCommandControl;
	mIsPermanent = false;	///< @todo make settable

	if( !deviceApiString.isEmpty() )
	{
		mApiB64 = deviceApiString.toUtf8().toBase64();
		mApiHash = QCryptographicHash::hash( mApiB64, QCryptographicHash::Md5 ).toHex();
	}
}

bool ClientCommandDeviceApi::isDataValid() const
{
	return ( QCryptographicHash::hash( mApiB64, QCryptographicHash::Md5 ).toHex() == mApiHash );
}

bool ClientCommandDeviceApi::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	mIsPermanent = cmdElement.attribute( "permanent", "false" ) == "true";
	mApiB64 = cmdElement.text().toUtf8();
	mApiHash = cmdElement.attribute( "hash" ).toUtf8();
	return isValid();
}

ClientCommandBase *ClientCommandDeviceApi::clone()
{
	return new ClientCommandDeviceApi();
}

ClientCommandBase *ClientCommandDeviceApi::exactClone()
{
	ClientCommandDeviceApi *clone = new ClientCommandDeviceApi();
	clone->mIsPermanent = mIsPermanent;
	clone->mApiB64 = mApiB64;
	clone->mApiHash = mApiHash;
	return clone;
}

QDomElement ClientCommandDeviceApi::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	QString permanentStr = mIsPermanent? "true" : "false";
	cmdElement.setAttribute( "permanent", permanentStr );
	cmdElement.setAttribute( "hash", QString::fromUtf8(mApiHash.data()) );
	cmdElement.appendChild( dom.createCDATASection( QString::fromUtf8(mApiB64.data()) ) );
	return cmdElement;
}

bool ClientCommandDeviceApi::isValid() const
{
	return ( !mApiB64.isEmpty() && !mApiHash.isEmpty() ) && ClientCommandBase::isValid();
}
