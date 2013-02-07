#include "DeviceStateVariableBase.h"

using namespace QtuC;

DeviceStateVariableBase::DeviceStateVariableBase(const DeviceStateVariableBase &otherVar) : ErrorHandlerBase(0)
{
	mName = otherVar.getName();
	mHwInterface = otherVar.getHwInterface();
	mType = otherVar.getType();
	mValue = otherVar.getValue();
	mAccessMode = otherVar.getAccessMode();
	mLastUpdate = otherVar.getLastUpdateTime();
}

bool DeviceStateVariableBase::checkBaseInitParams( const QString &varHwInterface, const QString &varName, const QString &varType )
{
	if( varName.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariableBase initialized without a name! Variable not created.", "checkBaseInitParams()", "DeviceStateVariableBase" );
		return false;
	}

	if( varHwInterface.isEmpty() )
	{
		error( QtWarningMsg, "DeviceStateVariableBase initialized without a hardwareInterface. Variable not created.", "checkBaseInitParams()", "DeviceStateVariableBase" );
		return false;
	}

	if( !( !varType.isEmpty() && isValidType(varType) ) )
	{
		error( QtWarningMsg, "DeviceStateVariableBase initialized without a valid type ("+varType+")! Variable not created.", "checkBaseInitParams()", "DeviceStateVariableBase" );
		return false;
	}

	return true;
}

DeviceStateVariableBase* DeviceStateVariableBase::init( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &accessModeStr )
{
	if( !checkBaseInitParams(varHwInterface, varName, varType) )
		{ return 0; }
	return new DeviceStateVariableBase( varHwInterface, varName, varType, accessModeStr );
}

DeviceStateVariableBase::DeviceStateVariableBase( const QString& varHwInterface, const QString& varName, const QString& varType, const QString &accessModeStr )
	: ErrorHandlerBase(0),
	  mLastUpdate(0)
{
	mName = varName;
	mHwInterface = varHwInterface;
	mType = stringToType(varType);

	accessMode_t accessMode = accessModeFromString( accessModeStr );
	if( accessMode == undefinedAccess )
	{
		error( QtWarningMsg, "Invalid string for accessMode, variable will be read-only", "DeviceStateVariableBase()" );
		accessMode = readAccess;
	}

	mAccessMode = accessMode;

	// Set the type of the value QVariant container
	mValue = QVariant( mType );
}

bool DeviceStateVariableBase::isValid() const
{
	bool valid = true;
	valid = valid && ( mType != QVariant::Invalid );
	valid = valid && ( !mName.isEmpty() );
	valid = valid && ( !mHwInterface.isEmpty() );
	valid = valid && ( mAccessMode != undefinedAccess );
	valid = valid && ( mValue.isValid() );
	return valid;
}

const QString DeviceStateVariableBase::getName() const
{
	return mName;
}

const QString DeviceStateVariableBase::getHwInterface() const
{
	return mHwInterface;
}

const QVariant DeviceStateVariableBase::getValue() const
{
	return mValue;
}

QVariant::Type DeviceStateVariableBase::getType() const
{
	return mType;
}

DeviceStateVariableBase::accessMode_t DeviceStateVariableBase::getAccessMode() const
{
	return mAccessMode;
}

bool DeviceStateVariableBase::setValue( const QVariant& newValue )
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

bool DeviceStateVariableBase::setValue( const QString& newValue )
{
	QVariant castedNewVal( variantFromString( newValue, mType ) );
	if( !castedNewVal.isValid() ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariableBase::setValue( int newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariableBase::setValue( uint newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariableBase::setValue( double newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

bool DeviceStateVariableBase::setValue( bool newValue )
{
	QVariant castedNewVal( newValue );
	if( !convertQVariant(castedNewVal, mType) ) return false;
	swapValue(castedNewVal);
	return true;
}

void DeviceStateVariableBase::updateFromSource(const QString &newValue)
{
	QVariant castNewRawVal( newValue );

	if( !castNewRawVal.convert(mType) )
	{
		errorDetails_t errDetails;
		errDetails.insert( "name", mName );
		errDetails.insert( "newValue", newValue );
		errDetails.insert( "to type",  QString(QVariant::typeToName(mType)) );
		error( QtWarningMsg, "Failed to convert new value from source!", "updateFromSource()", errDetails );
		return;
	}

	if( mValue != castNewRawVal )
	{
		mValue = castNewRawVal;
		mLastUpdate = QDateTime::currentMSecsSinceEpoch();
		emit updated();
		emitValueChanged();
	}
}

bool DeviceStateVariableBase::emitSendMe()
{
	if( isValid() )
	{
		if( mAccessMode & writeAccess )
		{
			emit sendMe();
			return true;
		}
		else
		{
			debug( debugLevelVerbose, QString("Variable (%1:%2) has no write access, don't emit sendMe()").arg(mHwInterface,mName), "emitSendMe()" );
			return false;
		}
	}
	else
	{
		debug( debugLevelVeryVerbose, QString("Variable (%1:%2) is invalid, don't emit sendMe()").arg(mHwInterface,mName), "emitSendMe()" );
		return false;
	}
}

void DeviceStateVariableBase::emitValueChanged()
{
	if( !isValid() )
	{
		error( QtWarningMsg, "StateVariable is invalid. Signals not sent. Variable: "+mName, "emitValueChanged()" );
		return;
	}
/// @todo (also:template function) don't try to convert to number if statevar type is string.... and vica versa, emit only type correct signals, and the ones that can be converted for sure.
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

QVariant DeviceStateVariableBase::variantFromString( const QString& strVal, QVariant::Type varType )
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
			castedNewVal = QVariant(strVal.toInt(&ok, 16));
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
			castedNewVal = QVariant( strVal.toUInt(&ok, 16) );
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

QVariant::Type DeviceStateVariableBase::stringToType( const QString& strType )
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

bool DeviceStateVariableBase::isValidType( const QString &typeStr )
{
	return (bool)stringToType(typeStr);
}

void DeviceStateVariableBase::swapValue( const QVariant &newValue )
{
	if( mValue != newValue )
	{
		mValue = newValue;
		emitValueChanged();
		emitSendMe();
	}
}

bool DeviceStateVariableBase::convertQVariant( QVariant &var, QVariant::Type toType )
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

qint64 DeviceStateVariableBase::getLastUpdateTime() const
{
	return mLastUpdate;
}

qint64 DeviceStateVariableBase::getAgeMs() const
{
	return QDateTime::currentMSecsSinceEpoch() - mLastUpdate;
}

DeviceStateVariableBase::accessMode_t DeviceStateVariableBase::accessModeFromString(const QString &modeStr)
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
