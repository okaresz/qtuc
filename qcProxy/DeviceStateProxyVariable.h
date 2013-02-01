#ifndef DEVICESTATEPROXYVARIABLE_H
#define DEVICESTATEPROXYVARIABLE_H

#include <DeviceStateVariableBase.h>

namespace QtuC
{
/// @todo proper up-to-date doc!
/** DeviceStateVariable class.
 * This class represents a state variable on the device.
 * Each variable must have a name, a corresponding valid hardware interface, and at least one type. Type can be string, boolean, int, and double.
 * Each variable holds two values. A device-side raw value and a user-side (converted) value. The raw value represents the value on the device, as sent in the command, but cast to the rawType of the variable.
 * The user-side value is converted from the rawValue, according to the conversion formula specified in the deviceAPI as a Qt script (if no script specified, value and rawvalue will be the same). The converted value can be used for example to display the value on a GUI.
 * <br>
 * <b>Recommended usage</b><br>
 * If a variable set command comes from the device, connect it to the setFromDevice() slot. This updates the rawValue, calculates the converted Value, and emits update() and all valueChanged signals.
 * Please note that setFromDevice() <i>does not emit valueChangedRaw signals</i>.<br>
 * valueChanged() signals can be connected to GUI slots. GUI valueChanged() signals should be connected to the setValue() slots of the variable. These slots update the value, calculate the rawValue,
 * and emit all valueChangedRaw(), all valueChanged() <b>and setOnDevice()</b> signals. You should always connect the actual command sending to setOnDevice().*/
class DeviceStateProxyVariable : public DeviceStateVariableBase
{
	Q_OBJECT

public:

	/** Copy constructor.*/
	DeviceStateProxyVariable( const DeviceStateProxyVariable &otherVar );

	/** Initialize a DeviceStateVariable.
	 *	This static function returns a pointer to a new DeviceStateVariable object on success, null pointer on failure.
	 * @param varHwInterface The hardware interface containing the variable.
	 * @param varName The name of the variable.
	 * @param varType Type of the variable. Valid values are: string, bool, boolean, int, double.
	 * @param varRawType Type of the raw variable. If omitted, this will be the same as varType.
	 * @param accessMode I/O access mode of the variable. `r`: read-only, `w`: write-only, `rw`: read/write. See deviceAPI.xml documentation for more info.
	 * @param convertScriptFromRaw FromRaw script string. Used to convert the value from device to user side.
	 * @param convertScriptToRaw ToRaw script string. Used to convert the value from user to device side.
	 * @return A pointer to a new DeviceStateVariable object on success, null pointer on failure.*/
	static DeviceStateProxyVariable* init( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& varRawType = QString(), const QString& accessModeStr = QString("r"), const QString convertScriptFromRaw = QString(), const QString convertScriptToRaw = QString() );

	/** Retuns whether the variable is valid.
	 * @returns True if the variable has a valid type, name, hardware interface and non-empty, valid values, otherwise returns false*/
	bool isValid() const;

	/** Get if the variable is null.
	  *	The variable is null if it's raw value has never bee initialized, so QVariant.isNull() returns true.
	  *	@return True if the variable is null, false otherwise.*/
	bool isNull() const
		{ return mRawValue.isNull(); }

	const QVariant getRawValue() const		///< Get the device-side raw value as a QVariant
		{ return mRawValue; }
	QVariant::Type getRawType() const	///< Get raw type of variable.
		{ return mRawType; }

	/** Get convert script.
	  *	@param fromRaw Set to true, to get the device-side -> user-side convert script, or false to get the other direction.
	  *	@return The convert script as a QSstring.*/
	const QString getConvertScript( bool fromRaw ) const;

	/** Get the raw value as a QString, ready to send to the device.
	  *	Do the device-specific formatting here...
	  *	@return The string formatted for the device.*/
	const QString getDeviceReadyString() const;

	/** Get the state of auto-update.
	  * @returns True if auto-update is active, false if not.*/
	bool isAutoUpdateActive() const;

	/** Get the interval of auto-update.
	  * @returns The interval of auto-update in milliseconds, even if auto-update is inactive.*/
	int getAutoUpdateInterval() const;

public slots:

	/** Start auto-update.
	  * Start the auto update cycle for this variable.
	  * The parameters for the auto-update can be set in the deviceAPI file. If there's no information for the update in deviceAPI,
	  * intervalMs parameter is omitted, and this is the first call of startAutoUpdate() (so there's no previous interval info), this function does nothing, and returns false.
	  * @param intervalMs The auto-update interval. See setAutoUpdateInterval(). This param overrides any possible deviceAPI data.
	  * @returns True if the updater starts, false otherwise.*/
	bool startAutoUpdate( quint32 intervalMs = 0 );

	/** Stop auto-update.
	  * If the auto-updater is inactive, this function does nothing.*/
	void stopAutoUpdate();

	/** Set the auto-update interval.
	 * This function can be called even if the auto-update is active. In that case, the interval is updated immediately.
	 * @param interval Interval in milliseconds, unsigned 32bit integer. Must be between min-max values (see minAutoUpdateInterval).
	 * @returns True if the interval is successfully changed, false otherwise.*/
	bool setAutoUpdateInterval( quint32 intervalMs );

	/** Set a value conversion script.
	  *	The script is checked for syntax errors.
	  * @param fromRaw Convert direction of the script: device (raw) to user side if true, user to device side if false.
	  *	@param scriptStr The script as a QString.
	  *	@return True if the script is valid and successfully registered as a convert script, false if it has syntax errors, or cannot be registered.*/
	bool setConvertScript( bool fromRaw, const QString &scriptStr );

	/** Set the raw value.
	  * @todo is it really necessary to convert the QVariant explicitly? The type-based constructors are enough tfor the correct type.
	  * @todo or at least make a template function for it.
	  * @param newRawValue The new raw value for the variable as a QVariant.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( const QVariant& newRawValue );

	/** Set the raw value.
	  * @param newRawValue Set a new raw value for the variable from a QString.
	  * The new type of the variable will be string.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( const QString& newRawValue );

	/** Set the raw value.
	  * @param newRawValue Set a new raw value for the variable from an integer.
	  * The new type of the variable will be int.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( int newRawValue );

	/** Set the raw value.
	  * @param newRawValue Set a new raw value for the variable from an unsigned integer.
	  * The new type of the variable will be uint.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( uint newRawValue );

	/** Set the raw value.
	  * @param newRawValue Set a new raw value for the variable from a double.
	  * The new type of the variable will be double.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( double newRawValue );

	/** Set the raw value.
	  * @param newRawValue Set a new raw value for the variable from a boolean.
	  * The new type of the variable will be bool.
	  * @returns True on success flase otherwise.*/
	bool setRawValue( bool newRawValue );

	/** Updates the rawValue of the variable as got from the device.
	 *	Call or connect this slot when you receive a set command from the device.
	 *	This function pdates the rawValue, calculates the converted Value, and emits update() and all valueChanged signals
	 *	@param newRawValue The new raw value (string) as parsed from the device command*/
	void updateFromDevice( const QString& newRawValue );

private slots:
	/** Calculate raw or user-side value based on the convert scripts.
	  *	@param fromRaw Direction: fromRaw if true, toRaw if false.
	  *	@return True on success, false otherwise.*/
	bool scriptConvert( bool fromRaw );

	bool calculateValue();		///< Calculate user-side value from current device-side raw value and emit valueChanged signals.
	bool calculateRawValue();	///< Calculate device-side raw value from current user-side value and emit valueChangedRaw signals.

	// Called when an auto-update tick occurs meaning the variable should be updated from the device.
	// Currently the timeout() signal of the timer is connected directly to updateMe().
	//void handleAutoUpdateTick();

signals:

	void valueChangedRaw( const QVariant& );	///< Emitted if rawValue has changed. @param QVariant The raw value as a QVariant.
	void valueChangedRaw( const QString& );		///< Emitted if rawValue has changed. @param QString The rawValue cast to a string.
	void valueChangedRaw( int );		///< Emitted if rawValue has changed. @param int The rawValue cast to an integer.
	void valueChangedRaw( uint );		///< Emitted if rawValue has changed. @param uint The rawValue cast to an unsigned integer.
	void valueChangedRaw( double );		///< Emitted if rawValue has changed. @param double The rawValue cast to a double.
	void valueChangedRaw( bool );		///< Emitted if rawValue has changed. @param bool The rawValue cast to a boolean.

private:
	/** A private constructor.
	 *	Use init() to create a new DeviceStateVariable.
	 * @param varHwInterface The hardware interface containing the variable.
	 * @param varName The name of the variable.
	 * @param varType Type of the variable. Valid values are: string, bool, boolean, int, double.
	 * @param varRawType Type of the raw variable. If omitted, this will be the same as vartype.
	 * @param accessMode I/O access mode of the variable.
	 * @param convertScriptFromRaw FromRaw script string. Used to convert the value from device to user side.
	 * @param convertScriptToRaw ToRaw script string. Used to convert the value from user to device side.*/
	DeviceStateProxyVariable( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& varRawType, const QString &accessModeStr, const QString convertScriptFromRaw, const QString convertScriptToRaw );

	QVariant::Type mRawType; ///< The raw type of the variable.
	QVariant mRawValue;		///< The raw value of the variable.
	QScriptEngine mConvertEngine;	///< The QScriptEngine used to convert the value between raw and converted.
	QString mConvertToRawScript;		///< toRaw script string. Used to convert the value from user to device side.
	QString mConvertFromRawScript;	///< fromRaw script string. Used to convert the value from device to user side.
	quint32 mAutoUpdateInterval;	///< Auto update interval, milliseconds, 32bit unsigned integer.
	QTimer* mAutoUpdateTimer;	///< Timer object for auto update.
	static quint32 minAutoUpdateInterval;	///< Minimum allowed interval of auto-update timer.

	void emitValueChangedRaw(); ///< Emit valueChangedRaw signals for all types.

	/** Swap current value for the passed QVariant, if different.
	  *	Also emit valueChanged signals, and call calculateRawValue().
	  *	@return True on success, false otherwise.*/
	void swapValue( const QVariant &newValue );

	/** Swap current raw value for the passed QVariant, if different.
	  *	Also emit valueChangedRaw and setOnDevice signals, and call calculateValue().
	  *	@return True on success, false otherwise.*/
	void swapRawValue( const QVariant &newRawVal );

};

}	//QtuC::

#endif //DEVICESTATEPROXYVARIABLE_H

