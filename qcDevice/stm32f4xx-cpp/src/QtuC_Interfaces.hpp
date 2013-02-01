#ifndef QTUC_INTERFACES_H
#define QTUC_INTERFACES_H

#include "QtuC_Tools.hpp"

namespace QtuC
{

/** Class for managing the software interfaces.
 *	An interface can be on any software layer. You only need to register an interface, if it must handle deviceCommands.
 *	You can register an interface with QtuC::Interfaces::register().
 *	Once you register it, QtuC::Interfaces::routeCmd() will call the static execCmd() method of the destination interface with the parsed command params.*/
class Interfaces
{
public:
	/** Structure to represent a qcInterface on the device.
	 *	name: Name of the interface.<br>
	 *	execCmd: Pointer to the static command execute method of the interface.*/
	struct interface_t
	{
		const char *name;
		bool (*execCmd)( QtuC::cmdType_t, char*, char* );
	};

	/** Register an interface as a device Interface.
	 *	@param name Name of the interface. Use the pointer to the static interfaceName member of the interface.
	 *	@param execCmdPtr Pointer to the static execCmd member of the interface. Will be called in command arrives for the interface.
	 *	@return True if the interface has successfully registered, otherwise false.
	 *	Register can fail if the params are invalid, or the qcInterface count reached the maximum. In the latter case, raise QtuC__Interfaces_Max_Interface_Count.*/
	static bool regInterface( const char *name, bool (* const &execCmdPtr)( QtuC::cmdType_t, char*, char* ) );

	/** Route an incoming command to the destination hardware interface.
	 *	@param cmdStr The raw command string.
	 *	@return True on success, false otherwise.*/
	static bool routeCmd( char *cmdStr );

	/** Get a an interface based on it's name.
	 *	@param hwiName Name of the interface.
	 *	@return Pointer to the qcInterface structure.*/
	static const interface_t *get( char *hwiName );

private:

	/** Maximum number of qcInterfaces.
	*	Ideally, this matches the number of current interfaces, but if you add a new one, don't forget to update this.*/
	static uint8_t const MMaxInterfaceCount = 5;

	static interface_t mInterfaceList[MMaxInterfaceCount];	///< List of hardware interface representations.
	static uint8_t mInterfaceCount;	///< Current number of hwInterfaces. Must be less than or equal to QtuC__Interfaces_Max_Interface_Count.
};

}	//QtuC::
#endif // QTUC_INTERFACES_H
