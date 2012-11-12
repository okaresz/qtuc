#include "ClientCommandReqDeviceApi.h"

using namespace QtuC;

ClientCommandReqDeviceApi::ClientCommandReqDeviceApi() :
	ClientCommandBase()
{
	mName = "reqDeviceAPI";
	mClass = clientCommandControl;
}

bool ClientCommandReqDeviceApi::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }
	return true;
}

ClientCommandBase *ClientCommandReqDeviceApi::clone()
{
	return new ClientCommandReqDeviceApi();
}

ClientCommandBase *ClientCommandReqDeviceApi::exactClone()
{
	return new ClientCommandReqDeviceApi();
}

QDomElement ClientCommandReqDeviceApi::getDomElement() const
{
	QDomDocument dom;
	return dom.createElement(mName);
}

bool ClientCommandReqDeviceApi::isValid() const
{
	return ClientCommandBase::isValid();
}
