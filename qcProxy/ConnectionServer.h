#ifndef CONNECTIONSERVER_H
#define CONNECTIONSERVER_H

#include "ClientConnectionManagerBase.h"
#include <QString>
#include <QTcpServer>
#include <QList>

namespace QtuC
{

class ConnectionServer : public QObject
{
	Q_OBJECT
public:

	ConnectionServer();

	/** Get a client.
	  * Get a ClientConnectionManagerBase object of a client, based on the client id.
	  * @param clientID ID of the client.
	  * @return The ClientConnectionManagerBase object of the client if id was a valid one, otherwise return 0.*/
	ClientConnectionManagerBase* getClient( QString clientID );

signals:
	/** Emitted if a new client has connected.
	  * The signal is emitted only after a successful handshake is made and the client object is created.
	  * @param newClient The new client object (of ClientConnectionManagerBase).*/
	void newClientConnected( ClientConnectionManagerBase* newClient );

private slots:
	/** Handle incoming connection.
	  * @param newClientSocket the new socket created for the incoming client connection.*/
	void handleNewConnection( QTcpSocket* newClientSocket );

private:
	QTcpServer* mTcpServer;	///< Holds the QTcpServer object
	QList<ClientConnectionManagerBase*> mClients;	///< The list of connected clients

};

}	//QtuC::
#endif //CONNECTIONSERVER_H

