#include "QtuC_Interfaces.hpp"
#include <string.h>

using namespace QtuC;

uint8_t Interfaces::mInterfaceCount = 0;
Interfaces::interface_t Interfaces::mInterfaceList[] = {};

bool Interfaces::regInterface( const char *name, bool (* const &execCmdPtr)( QtuC::cmdType_t, char*, char* ) )
{
	if( mInterfaceCount == MMaxInterfaceCount )
	{
		QtuC::Tools::sendMessage( QtuC::msgError, "Reached max interface count!" );
		return false;
	}

	if( !name || !execCmdPtr )
		{ return false; }

	mInterfaceList[mInterfaceCount].execCmd = execCmdPtr;
	mInterfaceList[mInterfaceCount].name = name;
	++mInterfaceCount;
	return true;
}

bool Interfaces::routeCmd( char *cmdStr )
{
	QtuC::cmdType_t cmdType;
	char *cmdHwi=0, *cmdVar=0, *cmdArg=0;

	if( !QtuC::Tools::parseCmd( cmdStr, cmdType, cmdHwi, cmdVar, cmdArg ) )
		{ QtuC::Tools::sendMessage( QtuC::msgError, "Invalid command" ); }

	//call the interface
	for( uint8_t i=0; i<mInterfaceCount; ++i )
	{
		if( strcmp( cmdHwi, mInterfaceList[i].name ) == 0 )
			{ return mInterfaceList[i].execCmd( cmdType, cmdVar, cmdArg ); }
	}
	QtuC::Tools::sendMessage( QtuC::msgError, "Interface ", cmdHwi, " not found" );
	return false;
}

const Interfaces::interface_t *Interfaces::get( char *hwiName )
{
	for( uint8_t i=0; i<mInterfaceCount; ++i )
	{
		if( strcmp( mInterfaceList[i].name, hwiName ) == 0 )
			{ return mInterfaceList+i; }
	}
	return 0;
}
