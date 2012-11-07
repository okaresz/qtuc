#include "DeviceConnectionManagerBase.h"

using namespace QtuC;

DeviceConnectionManagerBase::DeviceConnectionManagerBase( QObject *parent ) : ErrorHandlerBase(parent)
{

}

bool DeviceConnectionManagerBase::sendCommand(DeviceCommand *cmd)
{
	if( !cmd )
		{ return false; }
	DeviceCommandBuilder *cmdBuilder = new DeviceCommandBuilder( *((DeviceCommandBase*)cmd) );
	//cmd->deleteLater();
	return sendCommand( cmdBuilder );
}
