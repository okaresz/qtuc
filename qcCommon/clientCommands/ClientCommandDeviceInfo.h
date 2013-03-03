#ifndef CLIENTCOMMANDDEVICEINFO_H
#define CLIENTCOMMANDDEVICEINFO_H

#include "ClientCommandBase.h"
#include <QHash>

namespace QtuC
{

/** DeviceInfo command.
  *	The DeviceInfo command includes information about the currently connected device.*/
class ClientCommandDeviceInfo : public ClientCommandBase
{
	Q_OBJECT
public:
	explicit ClientCommandDeviceInfo();

	/** Set the list of informations to include in the command.
	  *	@param infoList List of key-value pairs of info.*/
	inline void setInfoList( QHash<QString,QString> const &infoList )
		{ mInfoList = infoList; }

	/** Get the list of device information included in the command.
	  *	@return the list of info.*/
	inline QHash<QString,QString> getInfoList() const
		{ return mInfoList; }

	/** Set devide startup time.
	  *	@param startupTime Devica startup time as a valid UNIX timestamp in milliseconds.*/
	inline void setStartupTime( qint64 startupTime )
		{ mStartupTime = startupTime; }

	/** Get device startup time.
	  *	@return Devica startup time as a valid UNIX timestamp in milliseconds.
	  *	If the command doesn't include this attribute, return 0.
	  *	If the command holds an invalid value, return 0, unless the startup flag is true, in which case the startup time is approximated with the arrival time of the command.*/
	inline qint64 getStartupTime() const
		{ return mStartupTime; }

	/** Set the startup flag.
	  *	...means that this command was triggered by a device startup.
	  * @param startup Startup flag to set.*/
	inline void setStartup( bool startup )
		{ mStartup = startup; }

	/** Get the startup flag of the command.
	  *	@return The startup flag. If true, this command was triggered by a device startup, if false, then most likely this command is a reply to a ClientCommandReqDeviceInfo.*/
	inline bool getStartup() const
		{ return mStartup; }

	/** Set the positiveAck flag.
	  *	@param posAck Value for the flag.*/
	inline void setPositiveAck( bool posAck )
		{ mPositiveAck = posAck; }

	/** Get the positiveAck flag.
	  *	@return The value included in the command.*/
	inline bool getPositiveAck() const
		{ return mPositiveAck; }

	/// @name Inherited methods from ClientCommandBase.
	/// @{
	bool applyDomElement( const QDomElement &cmdElement );
	ClientCommandBase *clone();
	ClientCommandBase *exactClone();
	QDomElement getDomElement() const;
	inline bool isValid() const
		{ return ClientCommandBase::isValid(); }
	/// @}

private:
	QHash<QString,QString> mInfoList;	///< List of device informations.
	qint64 mStartupTime;	///< Device startup time as UNIX timestamp in milliseconds.
	bool mStartup;		///< Whether this command was triggered by a device startup.
	bool mPositiveAck;	///< Whether the device uses positive acknowledgment.
};

}	//QtuC::
#endif // CLIENTCOMMANDDEVICEINFO_H
