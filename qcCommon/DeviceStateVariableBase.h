#ifndef DEVICESTATEVARIABLEBASE_H
#define DEVICESTATEVARIABLEBASE_H


#include <QObject>
#include <QString>
#include <QVariant>
#include "ErrorHandlerBase.h"
#include <QTimer>
#include <QDateTime>

namespace QtuC
{

/** DeviceStateVariableBase class.
 *	This class represents a state variable on the *source*. The *source* can be an embedded device, or a proxy, in any case a *source*, whose particular state should be mirrored in this variable.
 *	Each variable must have a name, a corresponding valid hardware interface, and a type. Type can be string, boolean, int, uint, and double.
 *	A variable also has an accessMode. See accessMode_t. <br>
 *	Internally, a stateVariable uses a QVariant to store the value. By default, till the first value assignment, the variable has *null* value (like QVariant). This can be checked with isNull().
 *	A variable holding a null value still can be valid.
 *	<br>
 * <b>Recommended usage</b><br>
 * Use the setValue() slots to set the value.
 * valueChanged() signals can be connected to GUI slots. GUI valueChanged() signals should be connected to the setValue() slots of the variable. These slots update the value,
 * and emit all valueChanged() signals <b>and sendMe()</b>. You should always connect the actual command sending towards the *source* to sendMe().*/
class DeviceStateVariableBase : public ErrorHandlerBase
{
	Q_OBJECT

public:

	/** I/O mode of the variable from the user's point of view.
	 *	Default is readAccess.
	 *	This is a maskable flag.
	 *	* `readAccess`: sendMe() signal will never be emitted.
	 *	* `writeAccess`: updateMe() signal will never be emitted.
	 *	* `readWriteAccess`: No restrictions, behaviour is full, bidirectional. (can be tested with the previous two, but here for convenience)*/
	enum accessMode_t
	{
		undefinedAccess = 0,
		readAccess = 4,
		writeAccess = 2,
		readWriteAccess = 6		// here for convenience...
	};

	/** Get accessMode from string representation.
	  *	@param modeStr The accessMode as a string.
	  *	@return The accessMode.*/
	static accessMode_t accessModeFromString( const QString &modeStr );

	/** Copy constructor.*/
	DeviceStateVariableBase( const DeviceStateVariableBase &otherVar );

	/** Check initializing parameters.
	 *	@return True if params are valid, false otherwise.*/
	static bool checkBaseInitParams( const QString &varHwInterface, const QString &varName, const QString &varType );

	/** Initialize a DeviceStateVariableBase.
	 *	This static function returns a pointer to a new DeviceStateVariableBase object on success, null pointer on failure.
	 *	@param varHwInterface The hardware interface containing the variable.
	 *	@param varName The name of the variable.
	 *	@param varType Type of the variable. Valid values are: string, bool, boolean, int, double.
	 *	@param accessModeStr I/O access mode string of the variable. `r`: read-only, `w`: write-only, `rw`: read/write. See deviceAPI.xml documentation for more info.
	 *	@return A pointer to a new DeviceStateVariableBase object on success, null pointer on failure.*/
	static DeviceStateVariableBase* init( const QString& varHwInterface, const QString& varName, const QString& varType, const QString& accessModeStr = QString("r") );

	/** Retuns whether the variable is valid.
	 *	A valid uninitialized variable (holding a null value) is considered to be valid.
	 * @returns True if the variable has a valid type, accessMode, name, hardware interface, otherwise returns false*/
	virtual bool isValid() const;

	const QString getName() const;			///< Get the name of the variable.
	const QString getHwInterface() const;	/// Get the hardware interface name of the variable.
	const QVariant getValue() const;		/// Get the user-side value as a QVariant
	QVariant::Type getType() const;	///< Get the <b>user-side</b> type of variable.
	accessMode_t getAccessMode() const;	///< Get the I/O access mode of the variable.

	/** Get if the variable is null.
	  *	The variable is null if it's value has never bee initialized, so QVariant.isNull() returns true.
	  *	@return True if the variable is null, false otherwise.*/
	virtual bool isNull() const
		{ return mValue.isNull(); }

	/** Get last update time.
	  * @returns The milliseconds (as a UNIX timestamp) when the variable was last updated (read from *source*).*/
	qint64 getLastUpdateTime() const;

	/** Get the number of milliseconds since the last update.
	 * ...meaning the time since the last set command for this var from the *source*.
	 * @returns The number of milliseconds since the last update.*/
	qint64 getAgeMs() const;

public slots:

	/** Set the value.
	  * @param newValue Set a new value for the variable from a QVariant.
	  * The new type of the variable will be the type of the variant.
	  * @returns True on success flase otherwise.*/
	bool setValue( const QVariant& newValue );

	/** Set the value.
	  * @param newValue Set a new value for the variable from a QString.
	  * The new type of the variable will be string.
	  * @returns True on success flase otherwise.*/
	bool setValue( const QString& newValue );

	/** Set the value.
	  * @param newValue Set a new value for the variable from an integer.
	  * The new type of the variable will be int.
	  * @returns True on success flase otherwise.*/
	bool setValue( int newValue );

	/** Set the value.
	  * @param newValue Set a new value for the variable from an unsigned integer.
	  * The new type of the variable will be uint.
	  * @returns True on success flase otherwise.*/
	bool setValue( uint newValue );

	/** Set the value.
	  * @param newValue Set a new value for the variable from a double.
	  * The new type of the variable will be double.
	  * @returns True on success flase otherwise.*/
	bool setValue( double newValue );

	/** Set the value.
	  * @param newValue Set a new value for the variable from a boolean.
	  * The new type of the variable will be bool.
	  * @returns True on success flase otherwise.*/
	bool setValue( bool newValue );

	/** Updates the value of the variable received from the *source*.
	 *	Call or connect this slot when you receive a set command from the *source*.
	 *	This function updates the value, emits updated() and all valueChanged signals
	 *	@param newValue The new value (string).*/
	virtual void updateFromSource( const QString& newValue );

protected slots:

signals:

	void sendMe();	///< Emitted if the variable should be sent to the source.

	void valueChanged( const QVariant& );	///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The rvalue as stored.
	void valueChanged( const QString& );	///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The value cast to a string.
	void valueChanged( int );			///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The value cast to an integer.
	void valueChanged( uint );			///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The value cast to an unsigned integer.
	void valueChanged( double );		///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The value cast to a double.
	void valueChanged( bool );		///< Emitted if value has changed. @note You should connect the GUI set* slots to one of these valueChanged() signals. @param The value cast to a boolean.

	/** Emitted if the variable gets updated from the *source*.
	 *	An update occurs when the proxy receives a set command from the *source*, and the received value differs from the current.*/
	void updated();

	/** Emitted if the variable should be updated.
	  *	If emitted, a get command must be sent to the *source* to request an up to date value for this variable.*/
	void updateMe();

protected:
	/** A protected constructor.
	 *	Use init() to create a new DeviceStateVariableBase.
	 *	@param varHwInterface The hardware interface containing the variable.
	 *	@param varName The name of the variable.
	 *	@param varType Type of the variable. Valid values are: string, bool, boolean, int, double.
	 *	@param accessModeStr I/O access mode of the variable.*/
	DeviceStateVariableBase( const QString& varHwInterface, const QString& varName, const QString& varType, const QString &accessModeStr );

	QString mName;			///< The name of the variable.
	QString mHwInterface;	///< The name of the hardware interface containing the variable.
	QVariant::Type mType;	///< The converted type of the variable.
	QVariant mValue;			///< The (converted) value of the variable.
	qint64 mLastUpdate;			///< UNIX millisec timestamp of the last update (from *source*).
	accessMode_t mAccessMode;	///< AccessMode of the variable.

	void emitValueChanged();	///< Emit valueChanged signals for all types.

	/** Check validity and access rights, then emit sendMe() signal.
	  *	@return True on success, false otherwise.*/
	bool emitSendMe();

	/** Create a QVariant from a string representation of the value, cast according to the given type.
	 * @param strVal String representation of the value.
	 * @param varType The casting is based on this type, and the returned QVariant will be of this type.
	 * @return A QVariant with the given varType type and the cast value, if the cast was successful, an invalid QVariant otherwise*/
	QVariant variantFromString( const QString& strVal, QVariant::Type varType );

	/** Convert a type string to QVariant::Type.
	 * Currently accepted values: string, int, double, bool, boolean.
	 * @param strType The string to convert.
	 * @return the corresponding QVariant::Type if the string could be converted, a QVariant::Invalid otherwise*/
	static QVariant::Type stringToType( const QString& strType );

	/** Check if passed string is a valid type name.
	  *	@param typeStr The string to check.
	  *	@return True if valid, false otherwise.*/
	static bool isValidType( const QString& typeStr );

	/** Swap current value for the passed QVariant, if different.
	  *	Also emit valueChanged signals.
	  *	@return True on success, false otherwise.*/
	virtual void swapValue( const QVariant &newValue );

	/** Convert QVariant between types.
	  *	@param var The QVariant to convert
	  *	@param toType Type to convert to.
	  *	@return True on success, false otherwise.*/
	bool convertQVariant( QVariant &var, QVariant::Type toType );

};

}	//QtuC::

#endif // DEVICESTATEVARIABLEBASE_H
