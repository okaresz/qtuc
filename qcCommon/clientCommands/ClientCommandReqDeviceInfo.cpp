#include "ClientCommandReqDeviceInfo.h"

using namespace QtuC;

ClientCommandReqDeviceInfo::ClientCommandReqDeviceInfo() :
	ClientCommandBase()
{
	mName = "reqDeviceInfo";
	mClass = clientCommandControl;
}

bool ClientCommandReqDeviceInfo::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }
	return true;
}

ClientCommandBase *ClientCommandReqDeviceInfo::clone()
{
	return new ClientCommandReqDeviceInfo();
}

ClientCommandBase *ClientCommandReqDeviceInfo::exactClone()
{
	return new ClientCommandReqDeviceInfo();
}

QDomElement ClientCommandReqDeviceInfo::getDomElement() const
{
	QDomDocument dom;
	return dom.createElement(mName);
}
