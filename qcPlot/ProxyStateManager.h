#ifndef PROXYSTATEMANAGER_H
#define PROXYSTATEMANAGER_H

#include "StateManagerBase.h"
#include "DeviceStatePlotDataVariable.h"
#include "DeviceStateHistoryVariable.h"

namespace qcPlot
{

class ProxyStateManager : public QtuC::StateManagerBase
{
	Q_OBJECT
public:
	explicit ProxyStateManager(QObject *parent = 0);

	static ProxyStateManager *instance( QObject *parent = 0 );

//	inline DeviceStatePlotDataVariable* getVar( const QString& hardwareInterface, const QString& varName )
//		{ return (DeviceStatePlotDataVariable*)(StateManagerBase::getVar(hardwareInterface,varName)); }

	inline DeviceStateHistoryVariable* getVar( const QString& hardwareInterface, const QString& varName )
		{ return (DeviceStateHistoryVariable*)(StateManagerBase::getVar(hardwareInterface,varName)); }

	/** @name Reimplemented
	  * @{*/
	bool registerNewStateVariable( QHash<QString,QString> params );
	/// @}

private:
	static ProxyStateManager *mInstance;
	
};

}	//QcPlot::
#endif // PROXYSTATEMANAGER_H
