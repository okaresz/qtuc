#include "DeviceAPIParser.h"
#include <QCryptographicHash>

using namespace QtuC;

DeviceAPIParser::DeviceAPIParser ( QObject* parent ) :
	ErrorHandlerBase(parent),
	mDocTypeName("QtuCDeviceAPIDef")
{}

DeviceAPIParser::~DeviceAPIParser()
{}

void DeviceAPIParser::clear()
{
	mCurrentApiHash.clear();
	mCurrentApiString.clear();
	debug( debugLevelVerbose, "Device API cleared", "clear()" );
}

/// @todo Create a checkAPI function and  rewrite the chaos of load/reload API... CheckAPI should include the checks in parseAPI (should we run the whole parse to catch semantic errors as well?...), and then in the future maybe DTD check.
bool DeviceAPIParser::parseAPI( const QByteArray& deviceAPIString )
{
	if( !isEmpty() )
	{
		error( QtWarningMsg, "You can only call parseAPI() on an empty DeviceAPIParser! Use clear() first.", "parseAPI" );
		return false;
	}

	QDomDocument apiMarkup;
	QString errMsg;
	int errLine;
	if( !apiMarkup.setContent( deviceAPIString, false, &errMsg, &errLine ) )
	{
		errorDetails_t errDetails;
		errDetails.insert( "message", errMsg );
		errDetails.insert( "line", QString::number(errLine) );
		error( QtCriticalMsg, "API parsing failed", "parseAPI(const QString&)", errDetails );
		return false;
	}

	if( apiMarkup.doctype().name() != mDocTypeName )
	{
		error( QtCriticalMsg, QString("API parsing failed: DTD name should be \"%1\".").arg( mDocTypeName ), "parseAPI(const QString&)" );
		return false;
	}

	if( apiMarkup.documentElement().tagName() != "deviceAPI" )
	{
		error( QtCriticalMsg, "API parsing failed: root node is not \"deviceAPI\"", "parseAPI(const QString&)" );
		return false;
	}

	QDomElement rootElement = apiMarkup.documentElement();
	QDomElement nextElement = rootElement.firstChildElement("deviceInfo");

	if( !nextElement.isNull() )
	{
		parseNodeDeviceInfo(nextElement);
	}
	else
		{ debug( debugLevelVerbose, "No deviceInfo node found! Consider including one. Continue parsing...", "parseAPI(const QString&)" ); }

	nextElement = rootElement.firstChildElement("hardwareInterfaceList");
	if( !nextElement.isNull() )
	{
		if( !parseNodeHardwareInterfaceList(nextElement) )
		{
			error( QtCriticalMsg, "API parsing failed while parsing hardwareInterfaceList, invalid API, ignored.", "parseAPI(const QString&)" );
			return false;
		}
	}
	else
	{
		error( QtCriticalMsg, "API parsing failed: missing hardwareInterfaceList, invalid API, ignored.", "parseAPI(const QString&)" );
		return false;
	}

	nextElement = rootElement.firstChildElement("stateVariableList");
	if( !nextElement.isNull() )
	{
		if( !parseNodeStateVariableList(nextElement) )
		{
			error( QtCriticalMsg, "API parsing failed while parsing stateVariableList, invalid API, ignored.", "parseAPI(const QString&)" );
			return false;
		}
	}
	else
	{
		error( QtCriticalMsg, "API parsing failed: missing stateVariableList, invalid API, ignored.", "parseAPI(const QString&)" );
		return false;
	}

	nextElement = rootElement.firstChildElement("functionList");
	if( !nextElement.isNull() )
	{
		if( !parseNodeFunctionList(nextElement) )
		{
			error( QtWarningMsg, "API parsing failed while parsing functionList", "parseAPI(const QString&)" );
		}
	}

	mCurrentApiHash = generateHash( deviceAPIString );
	mCurrentApiString = deviceAPIString;

	return true;
}

bool DeviceAPIParser::parseNodeDeviceInfo ( const QDomElement &deviceInfoElement )
{
	QDomElement infoElement = deviceInfoElement.firstChildElement();
	while( !infoElement.isNull() )
	{
		emit newDeviceInfo( infoElement.tagName(), infoElement.text() );
		infoElement = infoElement.nextSiblingElement();
	}
	return true;
}

bool DeviceAPIParser::parseNodeHardwareInterfaceList ( const QDomElement hardwareInterfaceListElement )
{
	QDomElement hwiElement = hardwareInterfaceListElement.firstChildElement("hardwareInterface");

	if( hwiElement.isNull() )
	{
		error( QtCriticalMsg, "No hardwareInterface node found in the device API!", "parseNodeHardwareInterfaceList()" );
		return false;
	}

	int hwiCount = 0;
	while( !hwiElement.isNull() )
	{
		if( hwiElement.firstChildElement("name").isNull() )
		{
			error( QtWarningMsg, QString("Missing hardware interface name node at line %1").arg(hwiElement.lineNumber()), "parseNodeHardwareInterfaceList()" );
			continue;
		}
		QString name = hwiElement.firstChildElement("name").text();
		QString info; hwiElement.firstChildElement("info").isNull()? info = "" : info = hwiElement.firstChildElement("info").text();
		emit newHardwareInterface( name, info );
		hwiCount++;
		hwiElement = hwiElement.nextSiblingElement("hardwareInterface");
	}

	if( hwiCount == 0 )
	{
		error( QtCriticalMsg, "No hardwareInterface registered", "parseNodeHardwareInterfaceList()" );
		return false;
	}

	return true;
}

bool DeviceAPIParser::parseNodeStateVariableList ( const QDomElement &stateVariableListElement )
{
	int varCount = 0;

	QDomElement stateVarElement = stateVariableListElement.firstChildElement("stateVariable");
	while( !stateVarElement.isNull() )
	{
		if( parseNodeStateVariable(stateVarElement) )
			{ varCount++; }
		stateVarElement = stateVarElement.nextSiblingElement("stateVariable");
	}

	if( varCount == 0 )
	{
		error( QtCriticalMsg, "No deviceStateVariable found in API definition!", "parseNodeStateVariableList()" );
		return false;
	}
	return true;
}

bool DeviceAPIParser::parseNodeFunctionList ( const QDomElement &functionListElement )
{
	QDomElement functionElement = functionListElement.firstChildElement("function");
	while( !functionElement.isNull() )
	{
		QString hwi, name;
		if( functionElement.firstChildElement("hwInterface").isNull() )
		{
			error( QtWarningMsg, QString("Missing hwInterface node for function at line %1, function skipped").arg(functionElement.lineNumber()), "parseNodeFunctionList()" );
			continue;
		}
		else
			{ hwi = functionElement.firstChildElement("hwInterface").text(); }

		if( functionElement.firstChildElement("name").isNull() )
		{
			error( QtWarningMsg, QString("Missing name node for function at line %1, function skipped").arg(functionElement.lineNumber()), "parseNodeFunctionList()" );
			continue;
		}
		else
			{ name = functionElement.firstChildElement("name").text(); }

		QString args;
		if( !functionElement.firstChildElement("args").isNull() )
			{ args = functionElement.firstChildElement("args").text(); }		// text() handles CDATA

		emit newDeviceFunction( hwi, name, args );

		functionElement = functionElement.nextSiblingElement("function");
	}
	return true;
}


bool DeviceAPIParser::parseNodeStateVariable( const QDomElement &stateVariableElement )
{
	// Check compulsory nodes

	if( stateVariableElement.firstChildElement("name").isNull() )
	{
		error( QtWarningMsg, "StateVariable parsing failed (on line "+QString::number(stateVariableElement.lineNumber())+"): no name node! Skip variable...", "parseNodeStateVariable()" );
		return false;
	}
	if( stateVariableElement.firstChildElement("hwInterface").isNull() )
	{
		error( QtWarningMsg, QString("StateVariable parsing failed (on line %1): no hwInterface node! Skip variable...").arg(stateVariableElement.lineNumber()), "parseNodeStateVariable()" );
		return false;
	}

	if( stateVariableElement.firstChildElement("type").isNull() )
	{
		error( QtWarningMsg, QString("StateVariable parsing failed (on line %1): no type node! Skip variable...").arg(stateVariableElement.lineNumber()), "parseNodeStateVariable()" );
		return false;
	}

	QHash<QString,QString> params;

	// Name, hwi

	params.insert( "name", stateVariableElement.firstChildElement("name").text() );
	params.insert( "hwInterface", stateVariableElement.firstChildElement("hwInterface").text() );

	// Type(s)

	QDomElement typeElement = stateVariableElement.firstChildElement("type");
	if( typeElement.firstChild().isText() )
		{ params.insert( "type", typeElement.text() ); }
	else
	{ // both are compulsory
		if( !typeElement.firstChildElement("device").isNull() )
			{ params.insert( "deviceType", typeElement.firstChildElement("device").text() ); }
		else
		{
			error( QtWarningMsg, QString("StateVariable type parsing failed (on line %1): missing device node! Skip variable...").arg(typeElement.lineNumber()), "parseNodeStateVariable()" );
			return false;
		}
		if( !typeElement.firstChildElement("user").isNull() )
			{ params.insert( "userType", typeElement.firstChildElement("user").text() ); }
		else
		{
			error( QtWarningMsg, QString("StateVariable type parsing failed (on line %1): missing user node! Skip variable...").arg(typeElement.lineNumber()), "parseNodeStateVariable()" );
			return false;
		}
	}

	// ... the rest is optional

	// Access

	QDomElement accessElement = stateVariableElement.firstChildElement("access");
	if( !accessElement.isNull() )
	{
		params.insert( "access-mode", accessElement.attribute( "mode", "r" ) );
	}

	// Conversion scripts

	if( !stateVariableElement.firstChildElement("conversion").isNull() )
	{
		QDomElement conversionElement = stateVariableElement.firstChildElement("conversion");
		if( conversionElement.firstChildElement("toUser").isNull() )
			{ error( QtWarningMsg, QString("Missing toUser node in conversion node for variable %1 on line %2").arg(params.value("name"),stateVariableElement.lineNumber()), "parseNodeStateVariable()" ); }
		else if( conversionElement.firstChildElement("toDevice").isNull() )
			{ error( QtWarningMsg, QString("Missing toDevice node in conversion node for variable %1 on line %2").arg(params.value("name"),stateVariableElement.lineNumber()), "parseNodeStateVariable()" ); }
		else
		{
			// text() handles CDATA
			params.insert( "toUserScript", conversionElement.firstChildElement("toUser").text() );
			params.insert( "toDeviceScript", conversionElement.firstChildElement("toDevice").text() );
		}
	}

	// Auto-update

	QDomElement autoUpdateElement = stateVariableElement.firstChildElement("autoUpdate");
	while( !autoUpdateElement.isNull() )
	{
		QString side = autoUpdateElement.attribute("side");
		if( side == "device" || side == "user" )
		{
			if( !autoUpdateElement.text().isEmpty() )
				{ params.insert( QString("autoUpdate-%1").arg(side), autoUpdateElement.text() ); }
			else
				{ error( QtWarningMsg, QString("autoUpdate is empty on line %2, for variable %1").arg(params.value("name"),autoUpdateElement.lineNumber()), "parseNodeStateVariable()" ); }
		}
		else
			{ error( QtWarningMsg, QString("side attribute is invalid on line %2 for variable %1 ").arg(params.value("name"),autoUpdateElement.lineNumber()), "parseNodeStateVariable()" ); }
		autoUpdateElement = autoUpdateElement.nextSiblingElement("autoUpdate");
	}

	/// @todo Implement guiHint

	QDomElement guiHintElement = stateVariableElement.firstChildElement("guiHint");
	if( !guiHintElement.isNull() )
	{
		params.insert( "guiHint", guiHintElement.text() );
	}

	emit newStateVariable( params );

	return true;
}

bool DeviceAPIParser::operator ==( const QString &deviceAPIString )
{
	return ( mCurrentApiHash == generateHash(deviceAPIString) );
}

const QByteArray DeviceAPIParser::getHash() const
{
	return mCurrentApiHash;
}

const QByteArray DeviceAPIParser::generateHash(const QString &apiString)
{
	return QCryptographicHash::hash( apiString.toAscii(), QCryptographicHash::Md5 );
}

const QString DeviceAPIParser::getApiString() const
{
	return mCurrentApiString;
}

bool DeviceAPIParser::setDocTypeName( QString const &name )
{
	if( !isEmpty() )
	{
		error( QtWarningMsg, "Try to set API DTD name on a non-empty parser object. Left unchanged.", "setDocTypeName()" );
		return false;
	}

	if( name.isEmpty() )
	{
		error( QtWarningMsg, "Try to set empty string as deviceAPI XML DTD name. Left unchanged.", "setDocTypeName()" );
		return false;
	}
	else
	{
		mDocTypeName = name;
		return true;
	}
}
