#include "ClientCommandDevice.h"
#include "DeviceStateVariableBase.h"

using namespace QtuC;

ClientCommandDevice::ClientCommandDevice( deviceCommandType_t type, const DeviceStateVariableBase *stateVariable ) :
	ClientCommandBase(),
	DeviceCommandBase()
{
	// Warn, but proceed.
	if( stateVariable && type == deviceCmdCall )
		{ error( QtWarningMsg, "A get ClientCommandDevice is created with a deviceVariable, something is wrong...", "ClientCommandDevice()" ); }

	commonConstruct(type);

	if( stateVariable )
	{
		mTimestamp = stateVariable->getLastUpdateTime();
		mHasTimestamp = true;	// stateVariable should always have a last update time
		mVariable = stateVariable->getName();
		mHwInterface = stateVariable->getHwInterface();
		/// @todo Send serialized QVariant, and check if local type is a match
		if( mType == deviceCmdSet )
			{ mArgs.append( stateVariable->getValue().toString() ); }
	}
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

	if( cmdElement.attribute( "time" ).isEmpty() )
	{
		mTimestamp = 0;
		mHasTimestamp = false;
	}
	else
	{
		bool ok;
		mTimestamp = cmdElement.attribute( "time" ).toLongLong( &ok, 16 );	// toLongLong() returns 0 on failure
		mHasTimestamp = ok;
	}

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
	return new ClientCommandDevice(this);
}

QDomElement ClientCommandDevice::getDomElement() const
{
	QDomDocument dom;
	QDomElement cmdElement = dom.createElement(mName);

	if( mType == deviceCmdSet && mHasTimestamp )
		{ cmdElement.setAttribute( "time", QString::number( mTimestamp, 16 ) ); }

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
	return ( ClientCommandBase::isValid() && DeviceCommandBase::isValid() );
}
