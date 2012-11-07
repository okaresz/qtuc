#include "ClientCommandDevice.h"

using namespace QtuC;

ClientCommandDevice::ClientCommandDevice( deviceCommandType_t type ) :
	ClientCommandBase(),
	DeviceCommandBase()
{
	commonConstruct(type);
}

ClientCommandDevice::ClientCommandDevice( DeviceCommandBase *deviceCommand ) :
	ClientCommandBase(),
	DeviceCommandBase(*deviceCommand)
{
	commonConstruct(deviceCommand->getType());
}

void ClientCommandDevice::commonConstruct(deviceCommandType_t type)
{
	mType = type;

	switch( mType )
	{
		case deviceCmdGet: mName = "get"; break;
		case deviceCmdSet: mName = "set"; break;
		case deviceCmdCall: mName = "call"; break;
		default: mName = "";	// will be invalid command
	}

	mClass = clientCommandDevice;
}

bool ClientCommandDevice::applyDomElement(const QDomElement &cmdElement)
{
	if( !checkTagName(cmdElement) )
		{ return false; }

	setInterface( cmdElement.attribute( "hwi" ) );
	if( mType == deviceCmdCall )
	{
		setFunction( cmdElement.attribute( "func" ) );
		if( mVariable.isEmpty() )
		{
			error( QtWarningMsg, "Invalid call command: missing function name", "applyDomElement()" );
			return false;
		}
	}
	else
	{
		setVariable( cmdElement.attribute( "var" ) );
	}

	if( cmdElement.hasChildNodes() )
	{
		QDomElement argNode = cmdElement.firstChildElement("arg");
		while( !argNode.isNull() )
		{
			if( !appendArg( argNode.text() ) )
			{
				errorDetails_t errDet;
				errDet.insert( "argument", argNode.text() );
				error( QtWarningMsg, "Ivalid argument in ClientCommandDevice (invalid command?)", "applyDomElement()", errDet );
			}
			argNode = argNode.nextSiblingElement("arg");
		}
	}

	if( mType == deviceCmdSet && mArgs.isEmpty() )
	{
		error( QtWarningMsg, "Invalid set command: missing argument text", "applyDomElement");
		return false;
	}

	return isValid();
}

ClientCommandBase *ClientCommandDevice::clone()
{
	return new ClientCommandDevice(mType);
}

ClientCommandBase *ClientCommandDevice::exactClone()
{
	ClientCommandDevice *clone = new ClientCommandDevice(mType);
	clone->mHwInterface = mHwInterface;
	clone->mVariable = mVariable;
	clone->mArgs = mArgs;
	return clone;
}

QDomElement ClientCommandDevice::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	if( !mHwInterface.isEmpty() )
		{ cmdElement.setAttribute( "hwi", mHwInterface ); }
	if( !mVariable.isEmpty() )
	{
		if( mType == deviceCmdCall )
			{ cmdElement.setAttribute( "func", mVariable ); }
		else
			{ cmdElement.setAttribute( "var", mVariable ); }
	}
	if( !mArgs.isEmpty() )
	{
		for( int i=0; i<mArgs.size(); ++i )
		{
			QDomElement argNode = dom.createElement("arg");
			argNode.appendChild( dom.createCDATASection(mArgs.at(i)) );
			cmdElement.appendChild( argNode );
		}
	}
	return cmdElement;
}

bool ClientCommandDevice::isValid()
{
	bool validity = true;
	validity = validity && mType != deviceCmdUndefined;
	validity = validity && !( mType == deviceCmdSet && (mArgs.isEmpty() || mHwInterface.isEmpty()) );
	validity = validity && !( mType == deviceCmdCall && (mArgs.isEmpty() || mHwInterface.isEmpty()) );
	return ( ClientCommandBase::isValid() && validity );
}
