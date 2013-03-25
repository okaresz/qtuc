#include "DeviceStateHistoryVariable.h"

using namespace qcPlot;

qint64 DeviceStateHistoryVariable::mDeviceStartupTime = 0;

DeviceStateHistoryVariable::DeviceStateHistoryVariable( const QString &varHwInterface, const QString &varName, const QString &varType, const QString &accessModeStr ) :
	QtuC::DeviceStateVariableBase( varHwInterface, varName, varType, accessModeStr ),
	mLogHistory(true)
{
	if( !(
		mType == QVariant::Int ||
		mType == QVariant::UInt ||
		mType == QVariant::Double ) )
	{
		error( QtWarningMsg, "History variable can only be of number-like type! Conversion will fail.", "DeviceStateHistoryVariable" );
	}
}

void DeviceStateHistoryVariable::clearHistory()
{
	mHistory.clear();
	mValueLimits.first = mValueLimits.second = .0;
	mTimestampLimits.first = mTimestampLimits.second = 0;
}

void DeviceStateHistoryVariable::updateFromSource(const QString &newValue)
{
	DeviceStateVariableBase::updateFromSource( newValue );
	if( mLogHistory )
		{ pushToHistory(); }
}

void DeviceStateHistoryVariable::swapValue(const QVariant &newValue)
{
	DeviceStateVariableBase::swapValue( newValue );
	if( mLogHistory )
		{ pushToHistory(); }
}

bool DeviceStateHistoryVariable::isValid() const
{
	return DeviceStateVariableBase::isValid() && (
		mType == QVariant::Int ||
		mType == QVariant::UInt ||
				mType == QVariant::Double );
}

void DeviceStateHistoryVariable::onDeviceStartup()
{
	/// @todo now what!?
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
				/// @todo what if this runs BEFORE device connection?
				if( mDeviceStartupTime == 0 )
				{
					mDeviceStartupTime = mLastUpdate;
					error( QtWarningMsg, "Device startup time is 0! Take the time of the first arriving command...", "pushToHistory()" );
				}
				updateValueLimits(dval);
				updateTimestampLimits(toDeviceTime(mLastUpdate));
				mHistory.append( QPair<qint64,double>( mLastUpdate, dval ) );
				emit historyUpdated();
			}
//			else
//				{ debug( QtuC::debugLevelVerbose, "Variable update timestamp equals the previous, skip history update.", "pushToHistory" ); }
		}
		else
			{ error( QtWarningMsg, "Cannot convert variable to double, history update failed.", "pushToHistory" ); }
	}
	else
	{ error( QtWarningMsg, "History variable can only be of number-like type! History is disabled.", "pushToHistory" ); }
}

void DeviceStateHistoryVariable::updateValueLimits(double newVal)
{
	if( newVal > mValueLimits.second )
		{ mValueLimits.second = newVal; }
	else if( newVal < mValueLimits.first )
		{ mValueLimits.first = newVal; }
}

void DeviceStateHistoryVariable::updateTimestampLimits(qint64 newStamp)
{
	if( newStamp > mTimestampLimits.second )
		{ mTimestampLimits.second = newStamp; }
	else if( newStamp < mTimestampLimits.first )
		{ mTimestampLimits.first = newStamp; }
}
