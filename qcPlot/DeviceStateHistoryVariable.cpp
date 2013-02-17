#include "DeviceStateHistoryVariable.h"

using namespace qcPlot;

qint64 DeviceStateHistoryVariable::mDeviceStartupTime = 0;

DeviceStateHistoryVariable::DeviceStateHistoryVariable( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &accessModeStr ) :
	QtuC::DeviceStateVariableBase( varHwInterface, varName, varType, accessModeStr )
{
	if( !(
		mType == QVariant::Int ||
		mType == QVariant::UInt ||
		mType == QVariant::Double ) )
	{
		error( QtWarningMsg, "History variable can only be of number-like type! Conversion will fail.", "DeviceStateHistoryVariable" );
	}
}

void DeviceStateHistoryVariable::updateFromSource(const QString &newValue)
{
	DeviceStateVariableBase::updateFromSource( newValue );
	pushToHistory();
}

void DeviceStateHistoryVariable::swapValue(const QVariant &newValue)
{
	DeviceStateVariableBase::swapValue( newValue );
	pushToHistory();
}

bool DeviceStateHistoryVariable::isValid() const
{
	return DeviceStateVariableBase::isValid() && (
		mType == QVariant::Int ||
		mType == QVariant::UInt ||
		mType == QVariant::Double );
}

void DeviceStateHistoryVariable::pushToHistory()
{
	if( isValid() )
	{
		bool ok;
		double dval = mValue.toDouble( &ok );
		if( ok )
		{
			if( mHistory.isEmpty() || mHistory.last().first != mLastUpdate )
			{
				mHistory.append( QPair<qint64,double>( mLastUpdate, dval ) );
				emit historyUpdated();
			}
			else
				{ debug( QtuC::debugLevelVerbose, "Variable update timestamp equals the previous, skip history update.", "pushToHistory" ); }
		}
		else
			{ error( QtWarningMsg, "Cannot convert variable to double, history update failed.", "pushToHistory" ); }
	}
	else
		{ error( QtWarningMsg, "History variable can only be of number-like type! History is disabled.", "pushToHistory" ); }
}
