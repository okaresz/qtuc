#include "ClientCommandDeviceApi.h"
#include <QCryptographicHash>

using namespace QtuC;

ClientCommandDeviceApi::ClientCommandDeviceApi( const QString &deviceApiString )
{
	mName = "deviceAPI";
	mClass = clientCommandControl;
	mIsPermanent = false;	///< @todo make settable

	mApiB64 = deviceApiString.toUtf8().toBase64();
	mApiHash = QCryptographicHash::hash( mApiB64, QCryptographicHash::Md5 );
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
	cmdElement.setAttribute( "hash", QString(mApiHash) );
	cmdElement.appendChild( dom.createCDATASection( mApiB64 ) );
	return cmdElement;
}

bool ClientCommandDeviceApi::isValid() const
{
	return ( !mApiB64.isEmpty() && !mApiHash.isEmpty() ) && ClientCommandBase::isValid();
}
