#ifndef DEVICEAPIPARSER_H
#define DEVICEAPIPARSER_H

#include <QObject>
#include <QString>
#include <QDomNode>
#include "ErrorHandlerBase.h"

namespace QtuC
{

class DeviceStateVariable;

class DeviceAPIParser : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** c'tor*/
	DeviceAPIParser ( QObject* parent );

	/** Parse passed string or try to load from file
	  *	@param deviceAPIString If passed, parse this string. This must be the full device API definition string, as in the deviceAPI.xml. The XML DTD is optional.
	  *	If omitted, the parser tries to locate the API file (using the API file path from settings).
	  *	@return True on success, false otherwise.*/
	bool parseAPI ( const QString& deviceAPIString = QString() );

signals:

	/** Emitted if a new device information is parsed.
	  *	@param key Name of the information.
	  *	@param value Value of the information.*/
	void newDeviceInfo( QString key, QString value );

	/** Emitted if a new hardware interface is parsed.
	  *	@param hwiName Name of the new hardware interface.
	  *	@param hwiInfo Short description of the hardware interface.*/
	void newHardwareInterface ( QString hwiName, QString hwiInfo );

	/** Emitted is a new state variable is parsed.
	  * @param params Hash of the new stateVariable parameters.*/
	void newStateVariable ( QHash<QString,QString> params );

	/** Emitted if a new device function is parsed.
	  *	@param hwInterface Hardware interface of the function.
	  *	@param name name of the function.
	  * @param args Argument string of th function.*/
	void newDeviceFunction( QString hwInterface, QString name, QString args );

private:

	/** Parse the deviceInfo node.
	 *	@param deviceInfoElement The deviceInfo node as a QDomElement.
	 *	@return True on success, false otherwise.*/
	bool parseNodeDeviceInfo ( const QDomElement &deviceInfoElement );

	/** Parse the hardwareInterfaceList node.
	 *	@param hardwareInterfaceListElement The hardwareInterfaceList node as a QDomElement.
	 *	@return True on success, false otherwise.*/
	bool parseNodeHardwareInterfaceList ( const QDomElement hardwareInterfaceListElement );

	/** Parse the stateVariableList node.
	 *	@param stateVariableListElement The stateVariableList node as a QDomElement.
	 *	@return True on success, false otherwise.*/
	bool parseNodeStateVariableList ( const QDomElement &stateVariableListElement );

	/** Parse the functionList node.
	 *	@param functionListElement The functionList node as a QDomNode.
	 *	@return True on success, false otherwise.*/
	bool parseNodeFunctionList ( const QDomElement &functionListElement );

	/** Parse the stateVariable node.
	  *	Parse the node and register a new DeviceStateVariable object.
	 *	@param stateVariableElement The stateVariable node as a QDomElement.
	 *	@return True on success, false otherwise.*/
	bool parseNodeStateVariable( const QDomElement &stateVariableElement );

	/** MD5 hash of the current API file.
	 */
	QString mCurrentAPIHash;

	/// @todo all tag name in a QHash?.....
	const QString mRootNodeName = 'deviceAPI';
	const QString mInfoNodeName = 'deviceInfo';
	const QString mHwiNodeName = 'hardwareInterfaceList';
	const QString mVarNodeName = 'stateVariableList';
	const QString mFuncNodeName = 'functionList';
};

}	//QtuC::
#endif //DEVICEAPIPARSER_H

