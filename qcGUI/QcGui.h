#ifndef QCGUI_H
#define QCGUI_H

#include "ErrorHandlerBase.h"
#include "ProxyStateManager.h"
#include "ProxyConnectionManager.h"
#include "DeviceAPIParser.h"
#include "DeviceStateVariable.h"

using namespace QtuC;

namespace qcGui
{

/** GUI client for QtuC, main class.
  *	QCGui Manages the connection to proxy, and serves as an underlying data model for the GUI.*/
class QcGui : public QtuC::ErrorHandlerBase
{
	Q_OBJECT
public:
	explicit QcGui(QObject *parent = 0);

	~QcGui();

	/** Connect to proxy.
	  *	Address and port is read from settings.
	  *	@return True on success, false otherwise.*/
	void connectProxy();

	/** Set device API.
	  *	Parse passed API and build and initialize the GUI for it.
	  *	This function can only be called when no API is currently set. To change to another API at once, use resetDeviceApi().
	  *	@param apiString The API to set.
	  *	@return True on success, false otherwise.*/
	bool setDeviceApi( const QString &apiString );

	/** Re-set device API.
	  *	Clear previous configuration then set the new.
	  *	This function tries to determine first, if the new API is valid, and only then make the reset,
	  *	but there is still a possibility the new Api will not function properly. In that case the previous configuration will be lost.
	  *	@param apiString The new API to set.
	  *	@return True on success, false otherwise.*/
	bool resetDeviceApi( const QString &apiString );

	/** Clear the current device API.
	 *	De-init the GUI and clear all states.*/
	void clearDeviceApi();

	/** Call device function.
	  *	@param hwInterface Hardware interface of the function.
	  *	@param name Name of the function.
	  *	@param argList Argument list for the function.
	  *	@return True if the given parameters are valid and the call can be sent to proxy. This does not mean that the function call itself was successful on the device.*/
	bool callDeviceFunction( const QString &hwInterface, const QString &name, const QStringList &argList = QStringList() );

signals:

	/** Emitted if a new Device State Variable is created.
	  *	@param newVar Pointer to the new variable.*/
	void deviceVariableCreated( const QtuC::DeviceStateVariable *newVar, const QString &guiHint );

	/** Emitted if a new Device Function is created.
	  *	@param hwInterface Hardware interface of the function.
	*	@param name Nme of the function.*/
	void deviceFunctionCreated( QString hwInterface, QString name );

	/// Emitted when a new device API has been successfully set.
	void deviceApiSet();

	/// Emitted if the current device API is cleared.
	void deviceApiCleared();
	
private slots:

	/** Handle incoming command from proxy.
	  *	@param cmd the incoming command.*/
	void handleCommand( ClientCommandBase *cmd );

	/// An error occured during connection initialization.
	void proxyConnectError();

	/// Proxy is connected, handle it.
	void proxyConnected();

	/// Proxy has disconnected, handle it.
	void proxyDisconnected();

	/// Proxy connection is ready, handle it.
	void proxyConnectionReady();

	/** Handle if a variable must be set on the device.
	  *	In our case, this means we must send a set command to the proxy.
	  *	@param stateVar The vriable to set.
	  *	@param newRawVal The new value.*/
	void handleSetVariableOnDeviceRequest( DeviceStateVariable *stateVar, QString newRawVal );

	/** Create new device state variable.
	  *	Creates a new variable in the stateManager and emit stateVariableCreated().
	  *	@param varParams Parameters of the variable, as built in QtuC::DeviceAPIParser::parseNodeStateVariable().*/
	void createDeviceVariable(QHash<QString,QString> varParams);

	/** Create new device function.
	  *	Creates a new device function and emit deviceFunctionCreated().
	  *	Parameters as of QtuC::DeviceAPIParser::newDeviceFunction() signal.*/
	void createDeviceFunction( QString hwInterface, QString name, QString args );

private:

	/** Handle device API client command if received.
	  *	@param apiCmd The received API command.
	  *	@return True if received API is valid and has been succesfully loaded, false otherwise.*/
	void handleDeviceApiCmd( ClientCommandDeviceApi *apiCmd );

	/** Handle device cmd.
	  *	@todo Temporary solution, for proxy passthrough mode.*/
	bool handleDeviceCmd( ClientCommandDevice *deviceCmd );

	QtuC::ProxyStateManager *mProxyState;
	QtuC::ProxyConnectionManager *mProxyLink;
	QtuC::DeviceAPIParser *mApiParser;
};

}	//QtuC::
#endif // QCGUI_H
