#ifndef CONNECTIONSERVER_H
#define CONNECTIONSERVER_H

//#include "ClientConnectionManagerBase.h"
#include "ErrorHandlerBase.h"
#include <QString>
#include <QTcpServer>
#include <QList>

namespace QtuC
{

class ClientConnectionManagerBase;
class ClientCommandBase;

class ConnectionServer : public ErrorHandlerBase
{
	Q_OBJECT
public:

	ConnectionServer( QObject *parent = 0 );
	~ConnectionServer();

	/** Get a client.
	  * Get a ClientConnectionManagerBase object of a client, based on the client id.
	  * @param clientID ID of the client.
	  * @return The ClientConnectionManagerBase object of the client if id was a valid one, otherwise return 0.*/
	ClientConnectionManagerBase* getClient( QString clientID );

	/** Get a client.
	  * Get a ClientConnectionManagerBase object of a client, based on the client number.
	  *	Client number is the index in the list of clents, in the order they were connected.
	  * @param clientNumber Number of the client. If omitted, the first client (number 0) is returned.
	  * @return The ClientConnectionManagerBase object of the client if clientNumber was a valid one, otherwise return 0.*/
	ClientConnectionManagerBase* getClient( int clientNumber = 0 );

	/** Start listening.
	  *	Host and port defined in settings.
	  *	@return True on success, false otherwise.*/
	bool startListening();

	/** Send a broadcast command.
	  *	The passed command will be sent to all clients.
	  *	@param cmd The command to send.
	  *	@return True if the command has been successfully sent to ALL clients, false otherwise.*/
	bool broadcast( ClientCommandBase *cmd );

signals:
	/** Emitted if a new client has connected.
	  * The signal is emitted only after a successful handshake is made and the client object is created.
	  * @param newClient The new client object (of ClientConnectionManagerBase).*/
	void newClientConnected( ClientConnectionManagerBase* newClient );

private slots:
	/** Handle incoming connection.
	 *	This slot should be connected to QTcpServer::newConnection() signal.*/
	void handleNewConnection();

	/** Called when a client has disconnected.*/
	void handleDisconnect();

private:
	QTcpServer* mTcpServer;	///< Holds the QTcpServer object
	QList<ClientConnectionManagerBase*> mClients;	///< The list of connected clients

};

}	//QtuC::
#endif //CONNECTIONSERVER_H

