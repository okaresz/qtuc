#ifndef QCGUI_H
#define QCGUI_H

#include "ErrorHandlerBase.h"
#include "ProxyStateManager.h"
#include "ProxyConnectionManager.h"

namespace QtuC
{

class QcGui : public ErrorHandlerBase
{
	Q_OBJECT
public:
	explicit QcGui(QObject *parent = 0);

	~QcGui();

	/** Connect to proxy.
	  *	Address and port is read from settings.
	  *	@return True on success, false otherwise.*/
	void connectProxy();

signals:
	
private slots:

	/** Handle incoming command from proxy.
	  *	@param cmd the incoming command.*/
	void handleCommand( ClientCommandBase *cmd );

	/// An error occured during connection initialization.
	void proxyConnectError();

	/// Proxy is connected, handle it.
	void proxyConnected();

	/// Proxy is connected, handle it.
	void proxyDisconnected();

private:

	ProxyStateManager *mProxyState;
	ProxyConnectionManager *mProxyLink;
};

}	//QtuC::
#endif // QCGUI_H
