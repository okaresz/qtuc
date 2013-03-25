#include "QtuC_Tools.hpp"

using namespace QtuC;

volatile uint16_t IRQDisableLevel = 0;
const char Tools::Endl = '\n';
const char Tools::CmdSep = ' ';

bool Tools::sendCommandBase( cmdType_t const type, const char *interface, const char *var, const char *arg1, const char *arg2, const char *arg3, const char *arg4, const char *arg5 )
{
	if( !commandTypeToString(type) || !interface || !var )
	{
		sendMessage( msgError, "[QcTools::sendCommand] invalid command" );
		return false;
	}

	/// @todo Cache the string to be sent and sent it in one command, only guard that with interrupt blocks
	IRQDIS();

	print( Tools::commandTypeToString(type) );

	// print timestamp
	if( QtuC::Conf::useCmdTimestamps )
		{ printTimeStamp(); }

	putChar( CmdSep );
	print( interface );
	putChar( CmdSep );
	print( var );

	if( arg1 )
	{
		putChar( CmdSep );
		print( arg1 );
	}
	if( arg2 )
	{
		putChar( CmdSep );
		print( arg2 );
	}
	if( arg3 )
	{
		putChar( CmdSep );
		print( arg3 );
	}
	if( arg4 )
	{
		putChar( CmdSep );
		print( arg4 );
	}
	if( arg5 )
	{
		putChar( CmdSep );
		print( arg5 );
	}

	putChar( Endl );

	IRQEN();
	return true;
}

//template<>
//bool Tools::sendCommand<const char*>( cmdType_t const type, const char *interface, const char *var, const argT *arg )
//	{ return sendCommandBase( type, interface, var, arg ); }

bool Tools::parseCmd( char *cmd, cmdType_t &type, char *&hwi, char *&var, char *&arg )
{
	type = cmdUndefined;

	// Parse command type
	char *tokPos = strtok( cmd, " " );
	if( strcmp( tokPos, "get" ) == 0 ) { type = cmdGet; }
	else if( strcmp( tokPos, "set" ) == 0 ) { type = cmdSet; }
	else if( strcmp( tokPos, "call" ) == 0 ) { type = cmdCall; }

	// parse interface name
	hwi = strtok( NULL, " " );

	//parse variable / function name
	var = strtok( NULL, " " );

	// the rest is argument
	arg = var+strlen(var)+1;

	return true;
}

bool Tools::parseArg( char *arg, char **argv, uint8_t &argc )
{
	if( !arg || !argv ) return false;
	char *argEnd = arg + strlen(arg);
	uint8_t maxArgCount = argc;
	argc = 0;
	char *partPtr;
	while( arg < argEnd )
	{
		if( *arg == '"' )
		{
			partPtr = strtok( arg, "\"" );
			arg = partPtr+ strlen(partPtr)+2;	// +2 to skip separator and second "
		}
		else
		{
			partPtr = strtok( arg, " " );
			arg = partPtr+ strlen(partPtr)+1;
		}
		argv[argc++] = partPtr;
		arg += strlen(partPtr)+1;	// +1 to skip separator
		if( argc == maxArgCount )
			{ break; }
	}
	if( argc == 0 )
		{ return false; }
	else
		{ return true; }
}

void Tools::sendMessage( messageType_t const msgType, char const *msgPart1, char const *msgPart2, char const *msgPart3, char const *msgPart4, char const *msgPart5, char const *msgPart6, char const *msgPart7, char const *msgPart8, char const *msgPart9, char const *msgPart10 )
{
	/// @todo Cache the string to be sent and sent it in one command, only guard that with interrupt blocks
	IRQDIS();

	print( Tools::commandTypeToString(cmdCall) );

	// print timestamp
	if( QtuC::Conf::useCmdTimestamps )
		{ printTimeStamp(); }

	putChar( CmdSep );
	print( QtuC::Conf::proxyInterfaceName );
	putChar( CmdSep );
	print( "message" );

	putChar( CmdSep );
	print( messageTypeToString( msgType ) );

	putChar( CmdSep );

	print( msgPart1 );

	if( msgPart2 )
		{ print( msgPart2 ); }
	if( msgPart3 )
		{ print( msgPart3 ); }
	if( msgPart4 )
		{ print( msgPart4 ); }
	if( msgPart5 )
		{ print( msgPart5 ); }
	if( msgPart6 )
		{ print( msgPart6 ); }
	if( msgPart7 )
		{ print( msgPart7 ); }
	if( msgPart8 )
		{ print( msgPart8 ); }
	if( msgPart9 )
		{ print( msgPart9 ); }
	if( msgPart10 )
		{ print( msgPart10 ); }

	putChar( Endl );

	IRQEN();
}

void Tools::sendGreeting( char const *deviceParams[], char const *greetingMsg )
{
	/// @todo Cache the string to be sent and sent it in one command, only guard that with interrupt blocks
	IRQDIS();

	print( Tools::commandTypeToString(cmdCall) );

	// print timestamp
	if( QtuC::Conf::useCmdTimestamps )
		{ printTimeStamp(); }

	putChar( CmdSep );
	print( QtuC::Conf::proxyInterfaceName );
	putChar( CmdSep );
	print( "greeting" );

	if( deviceParams )
	{
		putChar( CmdSep );
		uint8_t i = 0;
		while( deviceParams[i] )
		{
			if( i> 0 )
				{ putChar( CmdSep ); }

			putChar('"');
			print( deviceParams[i++] );
			putChar(':');
			if( deviceParams[i] )
				{ print( deviceParams[i++] ); }
			else
			{
				putChar('"');
				break;
			}
			putChar('"');
		}
	}

	if( greetingMsg )
	{
		putChar( CmdSep );
		putChar('"');
		print( "msg:" );
		print( greetingMsg );
		putChar('"');
	}

	putChar( Endl );

	IRQEN();
}

const char *Tools::messageTypeToString( messageType_t const msgType )
{
	switch( msgType )
	{
		case msgUndefined: return "???"; break;
		case msgInfo: return "inf"; break;
		case msgDebug: return "dbg"; break;
		case msgWarning: return "wrn"; break;
		case msgError: return "err"; break;
		default: return 0;
	}
}

const char* Tools::commandTypeToString( cmdType_t const cType )
{
	switch( cType )
	{
		case cmdGet: return "get";
		case cmdSet: return "set";
		case cmdCall: return "call";
		default: return 0;
	}
}

bool QtuC::isTrue( const char* str )
{
	return ( Tools::isArg( str, "on" ) || Tools::isArg( str, "1" ) || Tools::isArg( str, "true" ) );
}
