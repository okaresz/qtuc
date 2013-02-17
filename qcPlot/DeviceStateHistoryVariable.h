#ifndef DEVICESTATEHISTORYVARIABLE_H
#define DEVICESTATEHISTORYVARIABLE_H

#include <DeviceStateVariableBase.h>
#include <QList>
#include <QPair>

namespace qcPlot
{

/** Device state variable history.
*	Class to store all updates of a DeviceStateVariable. The updates are stored with a UNIX timestamp.*/
class DeviceStateHistoryVariable : public QtuC::DeviceStateVariableBase
{
	Q_OBJECT
public:
	explicit DeviceStateHistoryVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString &accessModeStr );

	/** Get timestamp limits.
	  *	Get the smallest and largest timestamps.
	  * @return The timestamp limits as a QPair. First is the oldest (smallest) stamp, the secondis the newest (largest).*/
	QPair<qint64, qint64> const & getTimestampLimits() const
		{ return mTimestampLimits; }

	/** Get value limits.
	  *	Get the smallest and largest value in the history.
	  * @return The value limits as a QPair. First is the smallest, the secondis the largest value.*/
	QPair<double, double> const & getValueLimits() const
		{ return mValueLimits; }

	/** Set device startup time.
	  *	@param Device startup time as a valid non-zero UNIX timestamp.*/
	static void setDeviceStartupTime( qint64 timestamp );

	/** Get device startup time.
	  *	@return Device startup time. May be zero, if it hasn't been set.*/
	static qint64 getDeviceStartupTime()
		{ return mDeviceStartupTime; }

	/** Get an update time in device time.
	 *	In other words, convert a UNIX timestamp to device timestamp, which is relative to the device startup time (but still millisecond based).
	 *	@warning If the device startup time is not set, or 0, the returned timestamp will be the same as the passed one.
	 *	@param timestampUNIX The UNIX timestamp to convert.
	 *	@return The timestamp in device time.*/
	static quint32 toDeviceTime( qint64 timestampUNIX )
		{ return timestampUNIX - mDeviceStartupTime; }

	/** @name Reimplemented from base.
	*	@{*/
	void updateFromSource( const QString& newValue );
	void swapValue( const QVariant &newValue );
	bool isValid() const;
	/// @}

signals:

	/// Emitted when the variable history is updated.
	void historyUpdated();

protected:

	void pushToHistory();

	QList<QPair<qint64,double> > mHistory;	///< History of this state variable. Stores all new updates with the update timestamp and the new value.
	QPair<qint64, qint64> mTimestampLimits;	///< Lower and upper limits (minimum and maximum) of the timestamp-range.
	QPair<double, double> mValueLimits;	///< Lower and upper limits (minimum and maximum) of the value-range.

	static qint64 mDeviceStartupTime;	///< Device startup time as a UNIX timestamp. This should be requested from the proxy before using this variable.

};

}	//qcPlot::
#endif // DEVICESTATEHISTORYVARIABLE_H
