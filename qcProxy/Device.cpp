#include "Device.h"
#include <QDebug>

using namespace QtuC;

Device *Device::mInstance = 0;
QStringList Device::mHardwareInterfaces = QStringList();
QStringList Device::mHardwareInterfaceInfo = QStringList();
QList<QStringList> Device::mFunctions = QList<QStringList>();
bool Device::mPositiveAck = false;
QHash<QString,QString> Device::mInfo = QHash<QString,QString>();

Device *Device::create(QObject *parent)
{
	if( mInstance )
	{
		error( QtWarningMsg, "Only one Device insatnce can exist at a time! Use clear() first!", "create()", "Device" );
		return 0;
	}
	mInstance = new Device(parent);
	return mInstance;
}

Device *Device::create( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent )
{
	if( mInstance )
	{
		error( QtWarningMsg, "Only one Device insatnce can exist at a time! Use clear() first!", "create()", "Device" );
		return 0;
	}

	if( hwInterfaceList.isEmpty() )
	{
		ErrorHandlerBase::error( QtCriticalMsg, "Cannot create Device with no hardware interface!", "create()", "Device" );
		return 0;
	}

	mInstance = new Device( hwInterfaceList, hwInterfaceInfoList, deviceName, parent);
	return mInstance;
}

Device::Device( QObject *parent ) : ErrorHandlerBase( parent ), mCreated(false)
{
	mInfo.insert( "name", QString() );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );

	mHardwareInterfaces.append( ":proxy" );
}

Device::Device( const QStringList& hwInterfaceList, const QStringList& hwInterfaceInfoList, const QString & deviceName, QObject *parent ) : ErrorHandlerBase( parent ), mCreated(false)
{
	mHardwareInterfaces = QStringList( hwInterfaceList );
	mHardwareInterfaceInfo = QStringList( hwInterfaceInfoList );

	mInfo.insert( "name", deviceName );
	mInfo.insert( "description", QString() );
	mInfo.insert( "platform", QString() );
	mInfo.insert( "project", QString() );

	mHardwareInterfaces.append( ":proxy" );
}

Device* Device::instance()
{
	if( mInstance->isCreated() )
		{ return 0; }
	else
	{
		error( QtWarningMsg, "Try to access Device instance, but it hasn't been created yet, use create()", "instance()", "Device" );
		return mInstance;
	}
}

Device* Device::swap(Device *newDevice)
{
	Device* oldInstance = mInstance;
	mInstance = newDevice;
	return oldInstance;
}

bool Device::isValidHwInterface( const QString& hwInterfaceName )
{
	return mHardwareInterfaces.contains( hwInterfaceName );
}

const QString Device::getHwInterfaceInfo( const QString& name )
{
	return mHardwareInterfaceInfo.at( mHardwareInterfaces.indexOf(name) );
}

const QString Device::getInfo(const QString &key)
{
	return mInfo.value( key, QString() );
}

const QString Device::getDescription()
{
	return mInfo.value( "description" );
}

const QString Device::getPlatform()
{
	return mInfo.value( "platform" );
}

const QString Device::getProject()
{
	return mInfo.value( "project" );
}

void Device::clear()
{
	this->disconnect();
	mHardwareInterfaces.clear();
	mHardwareInterfaceInfo.clear();
	mFunctions.clear();
	mInfo.clear();
	mPositiveAck = false;
	mCreated = false;
	mInstance->deleteLater();
	mInstance = 0;
}

deviceMessageType_t Device::messageTypeFromString(const QString &msgTypeStr)
{
	if( msgTypeStr == "info" ) { return deviceMsgInfo; }
	if( msgTypeStr == "debug" ) { return deviceMsgDebug; }
	if( msgTypeStr == "error" ) { return deviceMsgError; }
	return deviceMsgUndefined;
}

const QString Device::messageTypeToString(deviceMessageType_t msgType)
{
	switch( msgType )
	{
		case deviceMsgInfo: return QString("deviceMsgInfo");
		case deviceMsgDebug: return QString("deviceMsgDebug");
		case deviceMsgError: return QString("deviceMsgError");
		default: return QString("deviceMsgUndefined");
	}
}

const QString Device::getName()
{
	return mInfo.value( "name" );
}


void Device::addHardwareInterface(const QString &hwInterfaceName)
{
	if( !mCreated )
	{
		if( !mHardwareInterfaces.contains(hwInterfaceName) )
		{
			mHardwareInterfaces.append( hwInterfaceName );
			mHardwareInterfaceInfo.append(QString());
		}
	}
}

void Device::addFunction(const QString &hwInterface, const QString &name, const QString &args)
{
	QStringList funcDef;
	funcDef.append(hwInterface);
	funcDef.append(name);
	funcDef.append(args);
	mFunctions.append(funcDef);
}

void Device::setInfo(const QString &key, const QString &value)
{
	if( !mCreated )
	{
		if( key == "positiveAck" )
		{
			if( value == "true" || value == "1" || value == "on" )
				{ mPositiveAck = true; }
			else if( value == "false" || value == "0" || value == "off" )
				{ mPositiveAck = false; }
			else
				{ error( QtWarningMsg, "Invalid value set for positiveAck. Allowed values: true/false, on/off, 1/0. If parsed from API, check API file.", "setInfo()" ); }
		}
		else
			{ mInfo.insert( key, value ); }
	}
}

void Device::addHardwareInterfaceInfo(const QString &hwInterfaceName, const QString &hwInterfaceInfo)
{
	if( !mCreated )
	{
		if( !mHardwareInterfaces.contains(hwInterfaceName) )
		{
			mHardwareInterfaces.append( hwInterfaceName );
			mHardwareInterfaceInfo.append( hwInterfaceInfo );
		}
		else
			{ mHardwareInterfaceInfo.replace( mHardwareInterfaces.indexOf(hwInterfaceName), hwInterfaceInfo ); }
	}
}

void Device::setName( const QString & deviceName )
{
	if( !mCreated )
		{ mInfo.insert( "name", deviceName ); }
}

void Device::setDescription( const QString & deviceDesc )
{
	if( !mCreated )
		{ mInfo.insert( "description", deviceDesc ); }
}

void Device::setPlatform(const QString &platform)
{
	if( !mCreated )
		{ mInfo.insert( "platform", platform ); }
}

void Device::setProject(const QString &project)
{
	if( !mCreated )
		{ mInfo.insert( "project", project ); }
}

void Device::setPositiveAck(bool posAck)
{
	if( !mCreated )
		{ mPositiveAck = posAck; }
}
