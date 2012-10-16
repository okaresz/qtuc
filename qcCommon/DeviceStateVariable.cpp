#include "DeviceStateVariable.h"
#include <QDateTime>

using namespace QtuC;

DeviceStateVariable::init( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &varRawType, const QString convertScriptFromRaw, const QString convertScriptToRaw )
{
	if( varName.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a name! Variable not created.", "DeviceStateVariable::init" );
		return 0;
	}
	if( varHwInterface.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a hardwareInterface. Variable not created.", "DeviceStateVariable::init" );
		return 0;
	}
	if( !Device::isValidHwInterface(varHwInterface) )
	{
		error( QtWarningMsg, "Attempt to create a DeviceStateVariable with invalid hardware interface: "+varHwInterface+". Variable not created.", "DeviceStateVariable::init" );
		return 0;
	}
	if( !( !varType.isEmpty() && isValidType(varType) ) )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized without a valid type ("+varType+")! Variable not created.", "DeviceStateVariable::init" );
		return 0;
	}
	if( !varRawType.isEmpty() && !isValidType(varRawType) )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized an invalid rawType ("+varRawType+")! Variable not created.", "DeviceStateVariable::init" );
		return 0;
	}

	return new DeviceStateVariable( varHwInterface, varName, varType, varRawType, convertScriptFromRaw, convertScriptToRaw );
}

DeviceStateVariable::DeviceStateVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& varRawType, const QString convertScriptFromRaw, , const QString convertScriptToRaw ) : ErrorHandlerBase(0)
{
	mName = varName;
	mHwInterface = varHwInterface;
	mType = stringToType(varType);
	if( varRawType.isEmpty() )
		{ mRawType = mType; }
	else
		{ mRawType = stringToType(varRawType); }
	mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(mRawValue) );

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
	valid = valid && ( mRawValue.isValid() );
	valid = valid && ( !mRawValue.isNull() );
	valid = valid && ( mValue.isValid() );
	valid = valid && ( !mValue.isNull() );
	return valid;
}

void DeviceStateVariable::setFromDevice( const QString& newRawValue )
{
	QVariant castNewRawVal( variantFromString( newRawValue, rawType ) );

	if( !castNewRawVal.isValid() || castNewRawVal.isNull() )
	{
		QHash errDetails;
		errDetails.insert( "name", mName );
		errDetails.insert( "rawType", mRawType );
		errDetails.insert( "newRawValue", newRawValue );
		error( QtWarningMsg, "Invalid value from device!", "setFromDevice", errDetails );
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

bool DeviceStateVariable::setRawValue( const QVariant& newRawValue )
{
	if( !newRawValue.isValid() )
	{
		QHash errDet;
		errDet.insert( "varName", mName );
		errDet.insert( "newRawValue", newRawValue.toString() );
		error( QtWarningMsg, "New raw value is invalid.", errDet, "setRawValue(const QVariant&)" );
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
		QHash errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "newValue", newValue.toString() );
		error( QtWarningMsg, "New value is invalid.", errDetails, "setValue(const QVariant&)" );
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

DeviceStateVariable::emitValueChanged() const
{
	if( !isValid() )
	{
		error( QtWarningMsg, "StateVariable is invalid. Signals not sent. Variable: "+mName, "emitValueChanged()" );
		return;
	}

	bool ok = true;
	emit valueChanged( mValue );	//QVariant
	emit valueChanged( mValue.toString() );	//QString

	int v = mValue.toInt(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to int failed. Variable: %1, value: %2").arg( mName, mValue.toString() ) ); }
	else
		{ emit valueChanged( v ); }

	double vd = mValue.toDouble(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to double failed. Variable: %1, value: %2").arg( mName, mValue.toString() ) ); }
	else
		{ emit valueChanged( vd ); }

	//bool
	bool b = ( mValue.toBool() || mValue == "on" || mValue == "high" );
	emit valueChanged(b);
}

void DeviceStateVariable::emitValueChangedRaw() const
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
		{ error( QtWarningMsg, QString("StateVar conversion raw to int failed. Variable: %1, value: %2").arg( mName, mRawValue.toString() ) ); }
	else
		{ emit valueChangedRaw( v ); }

	double vd = mRawValue.toDouble(&ok);
	if( !ok )
		{ error( QtWarningMsg, QString("StateVar conversion raw to double failed. Variable: %1, value: %2").arg( mName, mRawValue.toString() ) ); }
	else
		{ emit valueChangedRaw( vd ); }

	//bool
	bool b = ( mRawValue.toBool() || mRawValue == "on" || mRawValue == "high" );
	emit valueChangedRaw(b);

	emit setOnDevice( getDeviceReadyString() );
}

bool DeviceStateVariable::scriptConvert( bool fromRaw )
{
	QString convertScript;
	QVariant::Type fromType, toType;
	QVariant fromValue, toValue;

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

	if( convertScript.isEmpty() )
	{
		if( fromType == toType )
		{
			toValue = fromValue;
		}
		else
		{
			QVariant tmpVar( fromValue );
			if( !tmpVar.convert(toType) ) return false;
			toValue = tmpVar;
		}
	}
	else
	{
		mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(fromType) );
		QVariant scriptReturn = mConvertEngine.evaluate(mConvertFromRawScript).toVariant();

		if( mConvertEngine.hasUncaughtException() )
		{
			QHash errDetails;
			errDetails.insert( "varName", mName );
			errDetails.insert( "line", mConvertEngine.uncaughtExceptionLineNumber() );
			errDetails.insert( "message", mConvertEngine.uncaughtException().toString() );
			QString dirStr = fromRaw? "fromRaw":"toRaw";
			error( QtWarningMsg, QString("Error in convert script (%1)!").arg(dirStr), "calculateValue()", errDetails );
			return false;
		}
		else
		{
			if( toType == scriptReturn.type() )
				{ toValue = scriptReturn; }
			else
			{
				if( !convertQVariant(scriptReturn, toType) ) return false;
				toValue = scriptReturn;
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
			castedNewVal = strVal;
			break;
		case QVariant::Int:
			bool ok;
			castedNewVal = strVal.toInt(&ok, 0);
			if( !ok )
			{
				QHash errDetails;
				errDetails.insert( "varName", mName );
				errDetails.insert( "strVal", strVal );
				errDetails.insert( "toType", QVariant::typeToName(varType) );
				error( QtWarningMsg, "Conversion from string failed.", "variantFromString()", errDetails );
				return QVariant();
			}
			break;
		case QVariant::Double:
			bool ok;
			castedNewVal = strVal.toDouble(&ok);
			if( !ok )
			{
				QHash errDetails;
				errDetails.insert( "varName", mName );
				errDetails.insert( "strVal", strVal );
				errDetails.insert( "toType", QVariant::typeToName(varType) );
				error( QtWarningMsg, "Conversion from string failed.", "variantFromString()", errDetails );
				return QVariant();
			}
			break;
		case QVariant::Bool:
			bool b = !( strVal == "false" || strVal == "off" || strVal == "low" || strVal == "0" );
			castedNewVal = b;
		default:
			error( QtWarningMsg, "Type ("+QVariant::typeToName(varType)+") is invalid", "variantFromString()" );
			return QVariant();
	}
	return castedNewVal;
}

QVariant::Type DeviceStateVariable::stringToType( const QString& strType )
{
	if( strType == "string" )
		{ return QVariant::String; }
	else if( strType == "int" )
		{ return QVariant::Int; }
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
	if( mRawValue != newRawValue )
	{
		mRawValue = newRawValue;
		/// @todo set update time??
		emitValueChangedRaw();
		emit setOnDevice(getDeviceReadyString());
		calculateValue();
	}
}

void DeviceStateVariable::swapValue( const QVariant &newValue )
{
	if( mValue != newValue )
	{
		mRawValue = newRawValue;
		emitValueChanged();
		calculateRawValue();
	}
}

bool DeviceStateVariable::convertQVariant( QVariant &var, QVariant::Type toType )
{
	if( fromType == toType ) return true;
	if( !var.convert(toType) )
	{
		QHash errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "value (converted to string)", var.toString() );
		errDetails.insert( "fromType", QVariant::typeToName(var.type()) );
		errDetails.insert( "toType", QVariant::typeToName(toType) );
		error( QtWarningMsg, "Value conversion failed!", "convertQVariant()", errDetails );
		return false;
	}
	return true;
}

qint64 DeviceStateVariable::getLastUpdateTime() const
{
	return mLastUpdate;
}

qint64 DeviceStateVariable::getAgeMs() const
{
	return QDateTime::currentMSecsSinceEpoch() - mLastUpdate;
}

bool DeviceStateVariable::startAutoUpdate()
{
	/// @todo Implement
}

bool DeviceStateVariable::setAutoUpdateFrequency( int freqHz )
{
	/// @todo Implement
}

void DeviceStateVariable::setAutoUpdate( bool state )
{
	/// @todo Implement
}

bool DeviceStateVariable::setConvertScript( bool fromRaw, const QString &scriptStr )
{
	QScriptSyntaxCheckResult syntaxCheck = mConvertEngine.checkSyntax(scriptStr);
	if( syntaxCheck.state() != QScriptSyntaxCheckResult::Valid )
	{
		QHash errDetails;
		errDetails.insert( "varName", mName );
		errDetails.insert( "line", syntaxCheck.errorLineNumber() );
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
	/// @todo Implement
}

bool DeviceStateVariable::getAutoUpdate() const
{
	/// @todo Implement
}

const QString DeviceStateVariable::getDeviceReadyString()
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
