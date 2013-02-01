/** @package QtuC
 *	The QtuC Device Library.
 *	Conventions:
 *	  * Static classes, private c'tor
 *	  * Static members
 *	    * If private, name is prefixed with "m", camelCase.
 *	    * If public, and can and should be used from outside the class, there's no prefix, and the first char is upper-case.
 *	    * private static const is prefixed with a capital "M", camelCase.
 *	  * Even though macros are global and are not part of a namespace (from the compiler's point of view), always include the macro definitions in the namespace block.
 */

#ifndef QTUC_TOOLS_H
#define QTUC_TOOLS_H

#include "stm32f4xx_conf.h"		/// this is only for convenience (as nearly all project files include QtuC_Tools), not needed directly by QtuC_Tools.
#include <string.h>
#include "UserCoreConfig.hpp"
#include "SysTime.hpp"

/** Convert integer to string.
 *	This function is written based on the itoa.c in the embox project. http://code.google.com/p/embox/, /trunk/embox/src/lib/stdlib/itoa.c.
 *	Convert integer to string using base. The result won't have any prefix, only the minus sign (if necessary), and the digits according to base.
 *	@param num Number to convert. Signed or unsigned.
 *	@param buf Buffer to store the converted string in. Be careful to always reserve enough space.
 *	@param base base for the conversion (radix). 1 < base < 17.*/
template<typename numT>
void itoa( numT const &num, char *buf, uint8_t const &base = 10 )
{
	char *p = buf;
	char *p1, *p2;
	numT ud = 0;
	*buf = '\0';	// initialize buffer. In the case of an error, this will already be in the buffer, indicating that the result is invalid (NULL).

	// check base
	if( base < 2 || base > 16 )
		{ return; }

	/* If num < 0, put `-' in the head.  */
	if( num < 0 )
	{
		*p++ = '-';
		buf++;
		ud = -num;
	}
	else
		{ ud = num; }

	/* Divide ud by base until ud == 0.  */
	uint32_t remainder = 0;
	do {
		remainder = ud % base;
		*p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
	} while( ud /= base );

	/* Terminate buf.  */
	*p = 0;

	/* Reverse buf.  */
	p1 = buf;
	p2 = p - 1;
	char tmp;
	while (p1 < p2) {
		tmp = *p1;
		*p1 = *p2;
		*p2 = tmp;
		p1++;
		p2--;
	}
}

template<typename T>
T abs( T const &num )
{
	if( num < 0 )
		{ return -num; }
	else
		{ return num; }
}

template<typename T>
inline bool isUintAddSafe( T const &int1, T const &int2 )
	{ return !( ( int1 & (1<<(sizeof(T)*8-1)) ) || ( int2 & (1<<(sizeof(T)*8-1)) ) ); }

namespace QtuC
{

namespace Conf
{
	uint8_t const maxCommandLength = 50;	///< Maximum length of incoming device commands.
	char const proxyInterfaceName[] = {":proxy"};		///< Hardware interface name for the special proxy interface (to send messages to qcProxy).
	bool const useCmdTimestamps = true;		///< Use timestamps in all commands. This way qcProxy is able to handle data points more precisely. You can read more about timekeeping in the Proxy documentation.
}

/// Define device command types
enum cmdType_t {
	cmdUndefined,
	cmdGet,
	cmdSet,
	cmdCall
};

/// Define device message types
enum messageType_t
{
	msgUndefined,
	msgInfo,
	msgDebug,
	msgWarning,
	msgError
};

/** Print function for QtuC::Tools.
 *	This function must be defined somewhere in the user code.
 *	QcTools will use this command as stdout/stderr.
 *	@param str String to print.*/
void print( char const *str );

/** PutChar function for QtuC::Tools.
 *	This function must be defined somewhere in the user code.
 *	QcTools will use this command to put characters to stdout/stderr.
 *	@param c Character to put.*/
void putChar( char const c );

/** Check if passed string evaluates to true.
 *	@param str String to check.
 *	@return True if string is "on", "true" or "1".*/
bool isTrue( char const *str );

/** QtuC Device Tools.
 *	Some helper functions to communicate with qcProxy.
 *	Most are used mainly by the framework, you will probably need sendGreeting(), isArg(), sendCommand(), sendMessage(), sendDebug(), or maybe parseArg()*/
class Tools
{
public:

	/** Parse command.
	 *	Tokenize command into parts.
	 *	@param cmd The command string to parse.
	 *	@warning cmd argument will be modified! (strtok())
	 *	@param type This output argument will be set to the command type.
	 *	@param hwi This output argument will be set to the command hardware interface.
	 *	@param var This output argument will be set to the command variable/function.
	 *	@param arg This output argument will be set to the start of the command argument(s).
	 *	@return True on success, false otherwise. */
	static bool parseCmd( char *cmd, cmdType_t &type, char *&hwi, char *&var, char *&arg );

	/** Wrapper for strcmp.
	 *	@return True if the two passed strings are equal, false if not.*/
	static inline bool isArg( char const *str1, char const *str2 )
		{ return ( strcmp(str1,str2) == 0 ); }

	/** Parse device command argument.
	 *	Tokenize argument by the argument separators and return the pointers to the argument parts.
	 *	@param arg The argument to parse. Have to be null terminated.
	 *	@warning arg Argument to parse. Will be modified (by strtok()).
	 *	@param argv Pointers to the parsed, separated argument parts: char *argv[n], where n should be >= than the number of expected arguments in arg.
	 *	@param argc Argument count. In this parameter you must pass the size of the provided argv array. This size will be modified to the actual number arguments.
	 *	@return True on success, false otherwise.*/
	static bool parseArg( char *arg, char **argv, uint8_t &argc );

	/** Send device greeting.
	 *	Send the greeting message. Ideally the greeting is sent when the device has successfully started and initialized, and ready for communication.
	 *	@param deviceParams Pointer to the array holding the device parameters (optional but recommended).
	 *		The parameter array is a string array with a sequence of { "param1_name", "param1_value", "param2_name", "param2_value", ... 0 }. The array must be terminated with a null pointer.
	 *		Any number of device parameter can be given with any name, they will be accessible in QtuC. Some parameters however are handled specially by QtuC (crossmatched with API, device identification, etc...):
	 *		  * *name*: device short name (possibly without any whitespace)
	 *		  * *desc*: Description of the device
	 *		  * *platform*: Platform and/or architercture of the device (eg. AVR-ATMega8, STM32F4xx, ...)
	 *		  * *project*: Possible identifier of the code running on the device, for example the name of the project, or the particular software (and version).
	 *	@param greetingMsg Some optional greeting message.*/
	static void sendGreeting( char const *deviceParams[], char const *greetingMsg = 0 );

	/** Send device command with an integer argument.
	 *	QtuC sends all integers as a prefixless (no "0x") lowercase hexadecimal number with a minus sign if arg < 0
	 *	@param type Type of the command.
	 *	@param interface The Hardware interface name.
	 *	@param var The command variable/function.
	 *	@param arg A signed or unsigned integer of any size.
	 *	@return True if command has been successfully sent, false otherwise.*/
	template<typename argT>
	static inline bool sendCommand( cmdType_t const type, const char *interface, const char *var, const argT &arg )
	{
		char buf[10];	// 8 for representing 32bit in base 16, plus sign, plus null terminator
		itoa( arg, buf, 16 );
		return sendCommandBase( type, interface, var, buf);
	}

	/** Send device command.
	 *	Overloading of the template -based sendCommand with const char*.
	 *	This method uses QtuC::print() to print the command.
	 *	@param type Type of the command.
	 *	@param interface The Hardware interface name.
	 *	@param var The command variable/function.
	 *	@param arg1 The optional command argument.
	 *	@param arg2 An optional second command argument.
	 *	@param arg3 An optional third command argument.
	 *	@param arg4 An optional fourth command argument.
	 *	@param arg5 An optional fifth command argument.
	 *	@return True if command has been successfully sent, false otherwise.*/
	static inline bool sendCommand( cmdType_t const type, const char *interface, const char *var, const char *arg1 = 0, const char *arg2 = 0, const char *arg3 = 0, const char *arg4 = 0, const char *arg5 = 0 )
		{ return sendCommandBase( type, interface, var, arg1, arg2, arg3, arg4, arg5); }

	/** Send a message.
	 *	Shortcut to send a device message with sendCommand().
	 *	@param msgType Message type.
	 *	@param msg msgPart1 Message part1.
	 *	@param msg msgPart2 Message part2, optional.
	 *	@param msg msgPart3 Message part3, optional.
	 *	@param msg msgPart4 Message part4, optional.
	 *	@param msg msgPart5 Message part5, optional.
	 *	@param msg msgPart6 Message part6, optional.
	 *	@param msg msgPart7 Message part7, optional.
	 *	@param msg msgPart8 Message part8, optional.
	 *	@param msg msgPart9 Message part9, optional.
	 *	@param msg msgPart10 Message part10, optional.*/
	static void sendMessage( messageType_t const msgType, char const *msgPart1, char const *msgPart2 = 0, char const *msgPart3 = 0, char const *msgPart4 = 0, char const *msgPart5 = 0, char const *msgPart6 = 0, char const *msgPart7 = 0, char const *msgPart8 = 0, char const *msgPart9 = 0, char const *msgPart10 = 0 );

	/** Send a debug message.
	 *	This is an overloaded template function.
	 *	Shortcut to send a device message with a *debug* message type and mixed arguments.
	 *	@param str1 Message part1, usually a first identifier.
	 *	@param int1 Message part2, usually the first data (integer).
	 *	@todo move somehow to cpp?...*/
	template<typename int1T>
	static inline void sendDebug( char const *str1, int1T const &int1 )
		{ sendDebug( str1, int1, 0, 0 );	}

	/** Send a debug message.
	 *	This is an overloaded template function.
	 *	Shortcut to send a device message with a *debug* message type and mixed arguments.
	 *	@param str1 Message part1, usually a first identifier.
	 *	@param int1 Message part2, usually the first data (integer).
	 *	@param str2 Message part3, usually the second identifier.
	 *	@param int2 Message part4, usually the second data (integer).
	 *	@todo move somehow to cpp?...*/
	template<typename int1T, typename int2T>
	static void sendDebug( char const *str1, int1T const &int1, char const *str2, int2T const &int2 )
	{
		print( Tools::commandTypeToString(cmdCall) );

		// print timestamp
		if( QtuC::Conf::useCmdTimestamps )
			{ printTimeStamp(); }

		putChar( CmdSep );
		print( QtuC::Conf::proxyInterfaceName );
		putChar( CmdSep );
		print( "message" );

		putChar( CmdSep );
		print( messageTypeToString( msgDebug ) );

		putChar( CmdSep );
		print( str1 );

		char buf[10];
		itoa( int1, buf, 16 );
		print( buf );

		if( str2 )
		{
			print( str2 );
			itoa( int2, buf, 16 );
			print( buf );
		}

		putChar( Endl );
	}

	/** Get command type as string.
	 *	@param cType Command type.
	 *	@return String representation of the command type.*/
	static const char* commandTypeToString( cmdType_t const cType );

	/** Get message type as string.
	 *  @param msgType Message type.
	 *	@return String representation of the message type.*/
	static const char* messageTypeToString( messageType_t const msgType );

	static const char Endl;			///< Endline constant
	static const char CmdSep;		///< Command part separator character.

private:

	/** Send device command.
	 *	This method uses QtuC::print() to print the command.
	 *	@param type Type of the command.
	 *	@param interface The Hardware interface name.
	 *	@param var The command variable/function.
	 *	@param arg1 The optional command argument.
	 *	@param arg2 An optional second command argument.
	 *	@param arg3 An optional third command argument.
	 *	@param arg4 An optional fourth command argument.
	 *	@param arg5 An optional fifth command argument.
	 *	@return True if command has been successfully sent, false otherwise.*/
	static bool sendCommandBase( cmdType_t const type, const char *interface, const char *var, const char *arg1 = 0, const char *arg2 = 0, const char *arg3 = 0, const char *arg4 = 0, const char *arg5 = 0 );

	static void inline printTimeStamp()
	{
		putChar( CmdSep );
		putChar( '@' );
		char buf[9];
		uint32_t us;
		SysTime::getUsSince( SysTime::NullTime, us );
		itoa( us, buf, 16 );
		print( buf );
	}
};

}	//QtuC::
#endif // QTUC_TOOLS_H
