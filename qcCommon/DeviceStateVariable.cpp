#include "DeviceStateVariable.h"
#include <QDateTime>

using namespace QtuC;

quint32 DeviceStateVariable::minAutoUpdateInterval = 10;

DeviceStateVariable::DeviceStateVariable(const DeviceStateVariable &otherVar) : ErrorHandlerBase(0)
{
	mName = otherVar.getName();
	mHwInterface = otherVar.getHwInterface();
	mRawType = otherVar.getRawType();
	mType = otherVar.getType();
	mRawValue = otherVar.getRawValue();
	mValue = otherVar.getValue();
	mAccessMode = otherVar.getAccessMode();
	mConvertToRawScript = otherVar.getConvertScript(false);
	mConvertFromRawScript = otherVar.getConvertScript(true);
	mLastUpdate = otherVar.getLastUpdateTime();
	mAutoUpdateInterval = otherVar.getAutoUpdateInterval();

	if( otherVar.isAutoUpdateActive() )
	{
		if( !startAutoUpdate() )
		{
			errorDetails_t errDetails;
			errDetails.insert( "name", mName );
			errDetails.insert( "hwi", mHwInterface );
			error( QtWarningMsg, "Unable to start auto-update", "DeviceStateVariable()", errDetails );
		}
	}

	mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(mRawValue) );
}

DeviceStateVariable* DeviceStateVariable::init( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &varRawType, const QString &accessModeStr, const QString convertScriptFromRaw, const QString convertScriptToRaw )
{
	if( varName.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a name! Variable not created.", "init()", "DeviceStateVariable" );
		return 0;
	}
	if( varHwInterface.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a hardwareInterface. Variable not created.", "init()", "DeviceStateVariable" );
		return 0;
	}
	/// @todo hope only this one (..is unreachable from gui prog, because Device is not in the common lib)
	/*if( !Device::isValidHwInterface(varHwInterface) )
	{
		error( QtWarningMsg, "Attempt to create a DeviceStateVariable with invalid hardware interface: "+varHwInterface+". Variable not created.", "init()", "DeviceStateVariable" );
		return 0;
	}*/
	if( !( !varType.isEmpty() && isValidType(varType) ) )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a valid type ("+varType+")! Variable not created.", "init()", "DeviceStateVariable" );
		return 0;
	}
	if( !varRawType.isEmpty() && !isValidType(varRawType) )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized an invalid rawType ("+varRawType+")! Variable not created.", "init()", "DeviceStateVariable" );
		return 0;
	}

	accessMode_t accessMode = accessModeFromString( accessModeStr );
	if( accessMode == undefinedAccess )
	{
		error( QtWarningMsg, "Invalid string for accessMode, variable will be read-only", "init()", "DeviceStateVariable" );
		accessMode = readAccess;
	}

	return new DeviceStateVariable( varHwInterface, varName, varType, varRawType, accessMode, convertScriptFromRaw, convertScriptToRaw );
}

DeviceStateVariable::DeviceStateVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& varRawType, accessMode_t accessMode, const QString convertScriptFromRaw, const QString convertScriptToRaw )
	: ErrorHandlerBase(0),
	  mLastUpdate(0),
	  mAutoUpdateInterval(0),
	  mAutoUpdateTimer(0)
{
	mName = varName;
	mHwInterface = varHwInterface;
	mType = stringToType(varType);
	mAccessMode = accessMode;
	if( varRawType.isEmpty() )
		{ mRawType = mType; }
	else
		{ mRawType = stringToType(varRawType); }
	mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(mRawValue) );

	// Set the type of the value QVariant containers
	mRawValue = QVariant( mRawType );
	mValue = QVariant( mType );

	if( !convertScriptFromRaw.isEmpty() )
		{ setConvertScript( true, convertScriptFromRaw ); }	//failure is not critical, let the variable be created...
	if( !convertScriptToRaw.isEmpty() )
		{ setConvertScript( false, convertScriptToRaw ); }
}

bool DeviceStateVariable::isValid() const
{
	bool valid = true;
	valid = valid && ( mRawType != QVariant::Invalid );
	valid = valid && ( mType != QVariant::Invalid );
	valid = valid && ( !mName.isEmpty() );
	valid = valid && ( !mHwInterface.isEmpty() );
	valid = valid && ( mAccessMode != undefinedAccess );
	valid = valid && ( mRawValue.isValid() );
	//valid = valid && ( !mRawValue.isNull() );
	valid = valid && ( mValue.isValid() );
	//valid = valid && ( !mValue.isNull() );
	return valid;
}

void DeviceStateVariable::setFromDevice( const QString& newRawValue )
{
	QVariant castNewRawVal( variantFromString( newRawValue, mRawType ) );

	if( !castNewRawVal.isValid() || castNewRawVal.isNull() )
	{
		errorDetails_t errDetails;
		errDetails.insert( "name", mName );
		errDetails.insert( "rawType",  QString(QVariant::typeToName(mRawType)) );
		errDetails.insert( "newRawValue", newRawValue );
		error( QtWarningMsg, "Invalid value from device!", "setFromDevice()", errDetails );
		return;
	}

	if( mRawValue != castNewRawVal )
	{
		mRawValue = castNewRawVal;
		mLastUpdate = QDateTime::currentMSecsSinceEpoch();
		emit updated(mRawValue);
		// Do not emit valueChangedRaw ( signal-slot recursion, because valueChangedRaw usually connected to setOnDevice(), however this connect - if exists - is outside  DeviceStateVar )
		// This will call valueUpdated signals
		calculateValue();
	}
}

const QString DeviceStateVariable::getName() const
{
	return mName;
}

const QString DeviceStateVariable::getHwInterface() const
{
	return mHwInterface;
}

const QVariant DeviceStateVariable::getRawValue() const
{
	 return mRawValue;
}

const QVariant DeviceStateVariable::getValue() const
{
	return mValue;
}

QVariant::Type DeviceStateVariable::getRawType() const
{
	return mRawType;
}

QVariant::Type DeviceStateVariable::getType() const
{
	return mType;
}

DeviceStateVariable::accessMode_t DeviceStateVariable::getAccessMode() const
{
	return mAccessMode;
}

const QString DeviceStateVariable::getConvertScript(bool fromRaw) const
{
	if( fromRaw )
		{ return mConvertFromRawScript; }
	else
		{ return mConvertToRawScript; }
}

bool DeviceStateVariable::setRawValue( const QVariant& newRawValue )
{
	if( !newRawValue.isValid() )
	{
		errorDetails_t errDet;
		errDet.insert( "varName", mName );
		errDet.insert( "newRawValue", newRawValue.toString() );
		error( QtWarningMsg, "New raw value is invalid.", "setRawValue(const QVariant&)", errDet );
		return false;
	}
	swapRawValue(newRawValue);
	return true;
}

bool DeviceStateVariable::setRawValue( const QString& newRawValue )
{
	QVariant castedNewRawVal( variantFromString( newRawValue, mRawType ) );
	if( !castedNewRawVal.isValid() ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateVariable::setRawValue( int newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateVariable::setRawValue( uint newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateVariable::setRawValue( double newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateVariable::setRawValue( bool newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateVariable::setValue( const QVariant& newValue )
{
	if( !newValue.isValid() )
	{
		errorDetails_t errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "newValue", newValue.toString() );
		error( QtWarningMsg, "New value is invalid.", "setValue(const QVariant&)", errDetails );
		return false;
	}
	QVariant convQVar(newValue);
	if( !convertQVariant(convQVar, mType) )
	{
		errorDetails_t errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "newValue", newValue.toString() );
		errDetails.insert( "type", QString(QVariant::typeToName(mType)) );
		error( QtWarningMsg, "New value cannot be converted to internal type.", "setValue(const QVariant&)", errDetails );
		return false;
	}
	swapValue(newValue);
	return true;
}

bool DeviceStateVariable::setValue( const QString& newValue )
{
	QVariant castedNewVal( variantFromString( newValue, mType ) );
	if( !castedNewVal.isValid() ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariable::setValue( int newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariable::setValue( uint newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariable::setValue( double newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariable::setValue( bool newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

void DeviceStateVariable::emitValueChanged()
{
	if( !isValid() )
	{
		error( QtWarningMsg, "StateVariable is invalid. Signals not sent. Variable: "+mName, "emitValueChanged()" );
		return;
	}
/// @todo (also:templyate function) don't try to convert to number if statevar type is string.... and vica versa, emit only type correct signals, and the ones that can be converted for sure.
	bool ok = true;
	emit valueChanged( mValue );	//QVariant
	emit valueChanged( mValue.toString() );	//QString

	int v = mValue.toInt(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion to int failed. Variable: %1, value: %2").arg( mName, mValue.toString() ), "emitValueChanged()" ); }
	else
		{ emit valueChanged( v ); }

	uint vu = mValue.toUInt(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion to uint failed. Variable: %1, value: %2").arg( mName, mValue.toString() ), "emitValueChanged()" ); }
	else
		{ emit valueChanged( vu ); }

	double vd = mValue.toDouble(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion to double failed. Variable: %1, value: %2").arg( mName, mValue.toString() ), "emitValueChanged()" ); }
	else
		{ emit valueChanged( vd ); }

	//bool
	bool b = ( mValue.toBool() || mValue == "on" || mValue == "high" );
	emit valueChanged(b);
}

void DeviceStateVariable::emitValueChangedRaw()
{
	if( !isValid() )
	{
		error( QtWarningMsg, "StateVariable is invalid. Signals not sent. Variable: "+mName, "emitValueChangedRaw()" );
		return;
	}

	bool ok = true;
	emit valueChangedRaw( mRawValue );	//QVariant
	emit valueChangedRaw( getDeviceReadyString() );	//QString

	int v = mRawValue.toInt(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to int failed. Variable: %1, value: %2").arg( mName, mRawValue.toString() ), "emitValueChangedRaw()" ); }
	else
		{ emit valueChangedRaw( v ); }

	uint vu = mRawValue.toUInt(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to uint failed. Variable: %1, value: %2").arg( mName, mRawValue.toString() ), "emitValueChangedRaw()" ); }
	else
		{ emit valueChangedRaw( vu ); }

	double vd = mRawValue.toDouble(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to double failed. Variable: %1, value: %2").arg( mName, mRawValue.toString() ), "emitValueChangedRaw()" ); }
	else
		{ emit valueChangedRaw( vd ); }

	//bool
	bool b = ( mRawValue.toBool() || mRawValue == "on" || mRawValue == "high" );
	emit valueChangedRaw(b);

	setOnDevice();
}

bool DeviceStateVariable::scriptConvert( bool fromRaw )
{
	QString *convertScript;
	QVariant::Type fromType, toType;
	QVariant *fromValue, *toValue;

	if( fromRaw )
	{
		convertScript = &mConvertFromRawScript;
		fromType = mRawType;
		fromValue = &mRawValue;
		toType = mType;
		toValue = &mValue;
	}
	else
	{
		convertScript = &mConvertToRawScript;
		fromType = mType;
		fromValue = &mValue;
		toType = mRawType;
		toValue = &mRawValue;
	}

	//----------------------------------------------

	if( convertScript->isEmpty() )
	{
		if( fromType == toType )
		{
			*toValue = *fromValue;
			return true;
		}
		else
		{
			QVariant tmpVar( *fromValue );
			if( !tmpVar.convert(toType) ) return false;
			*toValue = tmpVar;
			return true;
		}
	}
	else
	{
		mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(fromType) );
		QVariant scriptReturn = mConvertEngine.evaluate(*convertScript).toVariant();

		if( mConvertEngine.hasUncaughtException() )
		{
			errorDetails_t errDetails;
			errDetails.insert( "varName", mName );
			errDetails.insert( "line", QString::number(mConvertEngine.uncaughtExceptionLineNumber()) );
			errDetails.insert( "message", mConvertEngine.uncaughtException().toString() );
			QString dirStr = fromRaw? "fromRaw":"toRaw";
			error( QtWarningMsg, QString("Error in convert script (%1)!").arg(dirStr), "calculateValue()", errDetails );
			return false;
		}
		else
		{
			if( toType == scriptReturn.type() )
				{ *toValue = scriptReturn; }
			else
			{
				if( !convertQVariant(scriptReturn, toType) )
					{ return false; }
				*toValue = scriptReturn;
			}
			return true;
		}
	}
}

bool DeviceStateVariable::calculateValue()
{
	if( scriptConvert(true) )
	{
		emitValueChanged();
		return true;
	}
	else return false;
}

bool DeviceStateVariable::calculateRawValue()
{
	if( scriptConvert(false) )
	{
		emitValueChangedRaw();
		return true;
	}
	else return false;
}

QVariant DeviceStateVariable::variantFromString( const QString& strVal, QVariant::Type varType )
{
	if( strVal.isEmpty() )
	{
		error( QtWarningMsg, "New value is empty (varName:"+mName+")", "variantFromString()" );
		return QVariant();
	}

	QVariant castedNewVal;
	switch( varType )
	{
		case QVariant::String:
		{
			castedNewVal = QVariant(strVal);
			break;
		}
		case QVariant::Int:
		{
			bool ok;
			castedNewVal = QVariant(strVal.toInt(&ok, 0));
			if( !ok )
			{
				errorDetails_t errDetails;
				errDetails.insert( "varName", mName );
				errDetails.insert( "strVal", strVal );
				errDetails.insert( "toType", QVariant::typeToName(varType) );
				error( QtWarningMsg, "Conversion from string failed.", "variantFromString()", errDetails );
				return QVariant();
			}
			break;
		}
		case QVariant::UInt:
		{
			bool ok;
			castedNewVal = QVariant( strVal.toUInt(&ok, 0) );
			if( !ok )
			{
				errorDetails_t errDetails;
				errDetails.insert( "varName", mName );
				errDetails.insert( "strVal", strVal );
				errDetails.insert( "toType", QVariant::typeToName(varType) );
				error( QtWarningMsg, "Conversion from string failed.", "variantFromString()", errDetails );
				return QVariant();
			}
			break;
		}
		case QVariant::Double:
		{
			bool ok;
			castedNewVal = QVariant( strVal.toDouble(&ok) );
			if( !ok )
			{
				errorDetails_t errDetails;
				errDetails.insert( "varName", mName );
				errDetails.insert( "strVal", strVal );
				errDetails.insert( "toType", QVariant::typeToName(varType) );
				error( QtWarningMsg, "Conversion from string failed.", "variantFromString()", errDetails );
				return QVariant();
			}
			break;
		}
		case QVariant::Bool:
		{
			bool b = !( strVal == "false" || strVal == "off" || strVal == "low" || strVal == "0" );
			castedNewVal = QVariant( b );
			break;
		}
		default:
			error( QtWarningMsg, QString("Type (")+QVariant::typeToName(varType)+") is invalid", "variantFromString()" );
			return QVariant();
			break;
	}
	return castedNewVal;
}

QVariant::Type DeviceStateVariable::stringToType( const QString& strType )
{
	if( strType == "string" )
		{ return QVariant::String; }
	else if( strType == "int" )
		{ return QVariant::Int; }
	else if( strType == "uint" )
		{ return QVariant::UInt; }
	else if( strType == "double" )
		{ return QVariant::Double; }
	else if( strType == "bool" || strType == "boolean" )
		{ return QVariant::Bool; }
	else
		{ return QVariant::Invalid; }
}

bool DeviceStateVariable::isValidType( const QString &typeStr )
{
	return (bool)stringToType(typeStr);
}

void DeviceStateVariable::swapRawValue( const QVariant &newRawVal )
{
	if( mRawValue != newRawVal )
	{
		mRawValue = newRawVal;
		/// @todo set update time?? depends on positive ack?
		emitValueChangedRaw();
		setOnDevice();
		calculateValue();
	}
}

void DeviceStateVariable::swapValue( const QVariant &newValue )
{
	if( mValue != newValue )
	{
		mValue = newValue;
		emitValueChanged();
		calculateRawValue();
	}
}

bool DeviceStateVariable::convertQVariant( QVariant &var, QVariant::Type toType )
{
	if( var.type() == toType ) return true;
	if( !var.convert(toType) )
	{
		errorDetails_t errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "value (converted to string)", var.toString() );
		errDetails.insert( "fromType", QVariant::typeToName(var.type()) );
		errDetails.insert( "toType", QVariant::typeToName(toType) );
		error( QtWarningMsg, "Value conversion failed!", "convertQVariant()", errDetails );
		return false;
	}
	return true;
}

bool DeviceStateVariable::setOnDevice()
{
	if( isValid() )
	{
		if( mAccessMode & writeAccess )
		{
			emit setOnDevice(getDeviceReadyString());
			return true;
		}
		else
		{
			debug( debugLevelVerbose, QString("Variable (%1:%2) has no write access, won't set on device").arg(mHwInterface,mName), "setOnDevice()" );
			return false;
		}
	}
	else
	{
		debug( debugLevelVeryVerbose, QString("Variable (%1:%2) is invalid, won't set on device").arg(mHwInterface,mName), "setOnDevice()" );
		return false;
	}
}

qint64 DeviceStateVariable::getLastUpdateTime() const
{
	return mLastUpdate;
}

qint64 DeviceStateVariable::getAgeMs() const
{
	return QDateTime::currentMSecsSinceEpoch() - mLastUpdate;
}

bool DeviceStateVariable::startAutoUpdate( quint32 intervalMs )
{
	if( !(mAccessMode & readAccess) )
	{
		error( QtWarningMsg, QString("Cannot start auto-update, variable (%1:%2) is write-only").arg(mHwInterface,mName), "startAutoUpdate()" );
		return false;
	}

	if( !mAutoUpdateTimer )
		{ mAutoUpdateTimer = new QTimer(this); }

	if( intervalMs == 0 && mAutoUpdateInterval )
		{ intervalMs = mAutoUpdateInterval; }

	if( intervalMs == 0 )
	{
		error( QtWarningMsg, QString("Cannot start auto-update, interval is undefined (var: %1:%2").arg(mHwInterface,mName), "startAutoUpdate()" );
		return false;
	}

	if( !setAutoUpdateInterval( intervalMs ) )
	{
		error( QtWarningMsg, "Unable to set auto-update interval", "startAutoUpdate()" );
		return false;
	}

	connect( mAutoUpdateTimer, SIGNAL(timeout()), this, SIGNAL(updateMe()) );
	mAutoUpdateTimer->start();

	debug( debugLevelVerbose, QString("Auto-update started at a %3ms interval for variable %2:%1").arg(mName, mHwInterface, QString::number(mAutoUpdateInterval)), "startAutoUpdate()" );

	return true;
}

bool DeviceStateVariable::setAutoUpdateInterval( quint32 intervalMs )
{
	if( intervalMs == 0 )
	{
		error( QtWarningMsg, "Requested auto-update interval is zero, doing nothing", "setAutoUpdateInterval()" );
		return false;
	}

	if( intervalMs < minAutoUpdateInterval )
	{
		error( QtWarningMsg, QString("Requested auto-update interval is less than the minimum of %1ms").arg(QString::number(minAutoUpdateInterval)), "setAutoUpdateInterval()" );
		return false;
	}

	mAutoUpdateInterval = intervalMs;
	if( mAutoUpdateTimer )
		{ mAutoUpdateTimer->setInterval( intervalMs ); }

	return true;
}

bool DeviceStateVariable::setConvertScript( bool fromRaw, const QString &scriptStr )
{
	QScriptSyntaxCheckResult syntaxCheck = mConvertEngine.checkSyntax(scriptStr);
	if( syntaxCheck.state() != QScriptSyntaxCheckResult::Valid )
	{
		errorDetails_t errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "line", QString::number(syntaxCheck.errorLineNumber()) );
		errDetails.insert( "message", syntaxCheck.errorMessage() );
		QString dirStr = fromRaw? "fromRaw":"toRaw";
		error( QtWarningMsg, QString("Invalid conversion script syntax (%1)!").arg(dirStr), "setConvertScript()", errDetails );
		return false;
	}

	if( fromRaw )
		{ mConvertFromRawScript = scriptStr; }
	else
		{ mConvertToRawScript = scriptStr; }
	return true;
}

void DeviceStateVariable::stopAutoUpdate()
{
	mAutoUpdateTimer->stop();
}

bool DeviceStateVariable::isAutoUpdateActive() const
{
	return mAutoUpdateTimer->isActive();
}

int DeviceStateVariable::getAutoUpdateInterval() const
{
	return mAutoUpdateInterval;
}

const QString DeviceStateVariable::getDeviceReadyString() const
{
	switch(mRawType)
	{
		case QVariant::Int: return mRawValue.toString(); break;
		case QVariant::Double: return QString::number(mRawValue.toDouble(), 'f', 4); break;
		case QVariant::Bool: return mRawValue.toString(); break;
		case QVariant::String: return mRawValue.toString(); break;
		default: return QString();
	}
}

DeviceStateVariable::accessMode_t DeviceStateVariable::accessModeFromString(const QString &modeStr)
{
	if( modeStr == "r" )
		{ return readAccess; }
	else if( modeStr == "w" )
		{ return writeAccess; }
	else if( modeStr == "rw" )
		{ return readWriteAccess; }
	else
		{ return undefinedAccess; }
}
