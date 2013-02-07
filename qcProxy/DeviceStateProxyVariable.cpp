#include "DeviceStateProxyVariable.h"
#include "Device.h"

using namespace QtuC;

quint32 DeviceStateProxyVariable::minAutoUpdateInterval = 10;

DeviceStateProxyVariable::DeviceStateProxyVariable(const DeviceStateProxyVariable &otherVar) : DeviceStateVariableBase( otherVar )
{
	mRawType = otherVar.getRawType();
	mRawValue = otherVar.getRawValue();
	mConvertToRawScript = otherVar.getConvertScript(false);
	mConvertFromRawScript = otherVar.getConvertScript(true);
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

DeviceStateProxyVariable* DeviceStateProxyVariable::init( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &varRawType, const QString &accessModeStr, const QString convertScriptFromRaw, const QString convertScriptToRaw )
{
	if( !checkBaseInitParams( varHwInterface, varName, varType ) )
		{ return 0; }

	if( !Device::isValidHwInterface(varHwInterface) )
	{
		error( QtWarningMsg, "Attempt to create a DeviceStateVariable with invalid hardware interface: "+varHwInterface+". Variable not created.", "init()", "DeviceStateProxyVariable" );
		return 0;
	}

	if( !varRawType.isEmpty() && !isValidType(varRawType) )
	{
		error( QtWarningMsg, "DeviceStateVariable initialized an invalid rawType ("+varRawType+")! Variable not created.", "init()", "DeviceStateProxyVariable" );
		return 0;
	}

	return new DeviceStateProxyVariable( varHwInterface, varName, varType, varRawType, accessModeStr, convertScriptFromRaw, convertScriptToRaw );
}

DeviceStateProxyVariable::DeviceStateProxyVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& varRawType, const QString &accessModeStr, const QString convertScriptFromRaw, const QString convertScriptToRaw )
	: DeviceStateVariableBase( varHwInterface, varName, varType, accessModeStr ),
	  mAutoUpdateInterval(0),
	  mAutoUpdateTimer(0)
{
	if( varRawType.isEmpty() )
		{ mRawType = mType; }
	else
		{ mRawType = stringToType(varRawType); }
	mConvertEngine.globalObject().setProperty( mName, mConvertEngine.newVariant(mRawValue) );

	// Set the type of the value QVariant container
	mRawValue = QVariant( mRawType );

	if( !convertScriptFromRaw.isEmpty() )
		{ setConvertScript( true, convertScriptFromRaw ); }	//failure is not critical, let the variable be created...
	if( !convertScriptToRaw.isEmpty() )
		{ setConvertScript( false, convertScriptToRaw ); }
}

bool DeviceStateProxyVariable::isValid() const
{
	bool valid = true;
	valid = valid && DeviceStateVariableBase::isValid();
	valid = valid && ( mRawType != QVariant::Invalid );
	valid = valid && ( mRawValue.isValid() );
	//valid = valid && ( !mRawValue.isNull() );
	return valid;
}

void DeviceStateProxyVariable::updateFromDevice( const QString& newRawValue, const qint64 &timestamp )
{
	QVariant castNewRawVal( variantFromString( newRawValue, mRawType ) );

	if( !castNewRawVal.isValid() || castNewRawVal.isNull() )
	{
		errorDetails_t errDetails;
		errDetails.insert( "name", mName );
		errDetails.insert( "rawType",  QString(QVariant::typeToName(mRawType)) );
		errDetails.insert( "newRawValue", newRawValue );
		error( QtWarningMsg, "Invalid value from device!", "updateFromDevice()", errDetails );
		return;
	}

	if( mRawValue != castNewRawVal )
	{
		mRawValue = castNewRawVal;
		timestamp? mLastUpdate = timestamp : mLastUpdate = QDateTime::currentMSecsSinceEpoch();
		emit updated();
		// Do not emit valueChangedRaw ( signal-slot recursion, because valueChangedRaw usually connected to sendMe(), however this connect - if exists - is outside  DeviceStateVar )
		// This will call valueUpdated signals
		calculateValue();
	}
}

const QString DeviceStateProxyVariable::getConvertScript(bool fromRaw) const
{
	if( fromRaw )
		{ return mConvertFromRawScript; }
	else
		{ return mConvertToRawScript; }
}

bool DeviceStateProxyVariable::setRawValue( const QVariant& newRawValue )
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

bool DeviceStateProxyVariable::setRawValue( const QString& newRawValue )
{
	QVariant castedNewRawVal( variantFromString( newRawValue, mRawType ) );
	if( !castedNewRawVal.isValid() ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateProxyVariable::setRawValue( int newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateProxyVariable::setRawValue( uint newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateProxyVariable::setRawValue( double newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

bool DeviceStateProxyVariable::setRawValue( bool newRawValue )
{
	QVariant castedNewRawVal( newRawValue );
	if( !convertQVariant(castedNewRawVal, mRawType) ) return false;
	swapRawValue(castedNewRawVal);
	return true;
}

void DeviceStateProxyVariable::emitValueChangedRaw()
{
	if( !isValid() )
	{
		error( QtWarningMsg, "StateVariable is invalid. Signals not sent. Variable: "+mName, "emitValueChangedRaw()" );
		return;
	}

	emitSendMe();

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
	/// @todo variantFromString already does this in some way...
	bool b = ( mRawValue.toBool() || mRawValue == "on" || mRawValue == "high" );
	emit valueChangedRaw(b);
}

bool DeviceStateProxyVariable::scriptConvert( bool fromRaw )
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

bool DeviceStateProxyVariable::calculateValue()
{
	if( scriptConvert(true) )
	{
		emitValueChanged();
		return true;
	}
	else return false;
}

bool DeviceStateProxyVariable::calculateRawValue()
{
	if( scriptConvert(false) )
	{
		emitValueChangedRaw();
		return true;
	}
	else return false;
}

void DeviceStateProxyVariable::swapRawValue( const QVariant &newRawVal )
{
	if( mRawValue != newRawVal )
	{
		mRawValue = newRawVal;
		/// @todo set update time?? depends on positive ack?
		emitValueChangedRaw();
		calculateValue();
	}
}

void DeviceStateProxyVariable::swapValue( const QVariant &newValue )
{
	if( mValue != newValue )
	{
		mValue = newValue;
		emitValueChanged();
		calculateRawValue();
	}
}

bool DeviceStateProxyVariable::startAutoUpdate( quint32 intervalMs )
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

bool DeviceStateProxyVariable::setAutoUpdateInterval( quint32 intervalMs )
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

bool DeviceStateProxyVariable::setConvertScript( bool fromRaw, const QString &scriptStr )
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

void DeviceStateProxyVariable::stopAutoUpdate()
{
	mAutoUpdateTimer->stop();
}

bool DeviceStateProxyVariable::isAutoUpdateActive() const
{
	return mAutoUpdateTimer->isActive();
}

int DeviceStateProxyVariable::getAutoUpdateInterval() const
{
	return mAutoUpdateInterval;
}

const QString DeviceStateProxyVariable::getDeviceReadyString() const
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
