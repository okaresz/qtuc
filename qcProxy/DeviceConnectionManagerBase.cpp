#include "DeviceConnectionManagerBase.h"

using namespace QtuC;

DeviceConnectionManagerBase::DeviceConnectionManagerBase( QObject *parent ) : ErrorHandlerBase(parent)
{

}

DeviceConnectionManagerBase::~DeviceConnectionManagerBase()
{
	closeDevice();
}

DeviceCommand* DeviceConnectionManagerBase::buildCommandFromString( const QString &cmdStr )
{
	DeviceCommand *dc = new DeviceCommand( cmdStr );
	if( dc )
		{ return dc; }
	else
	{
		QHash<QString,QString> errDet;
		errDet.insert( "commandString", cmdStr );
		error( QtWarningMsg, "Invalid device command string", "buildCommand()", errDet );
		return 0;
	}
}
