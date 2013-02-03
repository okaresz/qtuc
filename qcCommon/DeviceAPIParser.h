#ifndef DEVICEAPIPARSER_H
#define DEVICEAPIPARSER_H

#include <QObject>
#include <QString>
#include <QDomNode>
#include "ErrorHandlerBase.h"

namespace QtuC
{

/** DeviceAPI Parser class.
*	Class to parse and handle the device API. Ideally there's only one instance of API parser in the application, no need for more.
*	This one instance stores the current API for future comparision and checking.
*	When the API is in string form, the XML DTD should always be included.*/
class DeviceAPIParser : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Creates an empty parser object.*/
	DeviceAPIParser ( QObject* parent = 0 );

	~DeviceAPIParser();

	/** Return if the parser object is empty.
	  *	Object is empty when there's no stored API.
	  *	@return True if empty, false if not.*/
	virtual bool isEmpty() const
		{ return mCurrentApiHash.isEmpty() && mCurrentApiString.isEmpty(); }

	/** Clear this API parser object.
	  *	Clear and discard the stored API.*/
	virtual void clear();

	/** Parse passed string or try to load from file
	  *	This function can only be called if this parser object is empty (see isEmpty()).
	  *	If you would like to replace the API, use clear() first, to discard the current API.
	  *	@param deviceAPIString If passed, parse this string. This must be the full device API definition string, as in the deviceAPI.xml. The XML DTD is optional.
	  *	@return True on success, false otherwise.*/
	bool parseAPI( const QByteArray &deviceAPIString );

	/** Overloaded parseAPI() for QString.
	  *	@param deviceAPIString The QString is converted to UTF-8 QByteArray with QString::toUtf8() and passed to parseAPI(const QByteArray&).*/
	bool parseAPI( const QString &deviceAPIString )
		{ return parseAPI( deviceAPIString.toUtf8() ); }

	/** Generate the API hash of the passed deviceAPI string.
	  *	@param apiString The deviceAPI string (content of the deviceAPI.xml, including the DTD).
	  *	@return The hash as a QByteArray.*/
	static const QByteArray generateHash( const QString &apiString );

	/** Get the hash of current API.
	  *	@return Hash of the current API as a QByteArray. If API is invalid, returns an empty QByteArray.*/
	const QByteArray getHash() const;

	/** Get the current API string.
	  *	@return Current API as a QString. If API is invalid, returns an empty QString.*/
	const QString getApiString() const;

	/** Just for your convenience.
	  *	Generate API hash from the passed string, and compare it to the self hash.
	  *	@param deviceAPIString API string to compare (content of the deviceAPI.xml, including the DTD).
	  *	@return True if deviceAPIString equals the current, false otherwise.*/
	bool operator==( const QString &deviceAPIString );

	/** Set API DTD name.
	  *	A new DTD name can only be set on an empty parser object. See isEmpty().
	  *	@param name The new DTD name. Must be valid by XML standards.*/
	bool setDocTypeName( QString const &name );

	/** Get API DTD name.
	  *	@return The API DTD name.*/
	QString const getDocTypeName()
		{ return mDocTypeName; }

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
	  * @param args Argument string of the function.*/
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

	QByteArray mCurrentApiHash;	///< MD5 hash of the current API string.
	QString mCurrentApiString;	///< Current API string.
	QString mDocTypeName;	///< DTD name of the deviceAPI. Parsed APIs are checked against this.
};

}	//QtuC::
#endif //DEVICEAPIPARSER_H

