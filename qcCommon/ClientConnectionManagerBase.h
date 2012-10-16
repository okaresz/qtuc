#ifndef CLIENTCONNECTIONMANAGERBASE_H
#define CLIENTCONNECTIONMANAGERBASE_H

#include <QString>

namespace QtuC
{

enum connectionState_t
{
	undefined_connection,
	unInitialized_connection,
	connected_connection,
	handShaking_connection,
	ready_connection,
	lost_connection,
	disconnected_connection,
	error_connection
};

/** Class ClientConnectionManagerBase.
 * Provides a basic interface to communicate with a client.
 * Specific Client connection manager classes should inherit this.*/
class ClientConnectionManagerBase
{
public:
	ClientConnectionManagerBase();

	void commandReceived( ClentCommandBase cmd );

	/**
	 * Commands should be created with new on the heap.
	 */
	bool sendCommand( ClentCommandBase * cmd );

	/**
	 * Send multiple commands of the same class in one packet.
	 */
	bool sendCommands( QList<ClentCommandBase*> cmdList );

	bool isConnected() const;

	/**
	 * Returns whether the conection is set up and client is ready to receive commands.
	 */
	bool isReady() const;


	/**
	 * Return de client's ID string.
	 */
	const QString getID() const;

	void sendHandShake();

	/**
	 * Send the deviceAPI definition to the GUI.
	 */
	void cmdSendDeviceAPIDef( const QString& apiDefString );

	/**
	 * if ackID is 0, ...then the heartbeat is invalid, because we can only acknowledge a client ack.
	 */
	connectionState getState() const;

	/**
	 *
	 */
	const QHash<QString,QString> getClientInfo();

	/**
	 *
	 */
	void setSelfInfo( const QString & key, const QString & value );

protected:
	static  qint64 packetCount;
	static  const QHash<QString,QString> selfInfo;
	QHash<QString,QString> clientInfo;
	QTcpSocket* clientSocket;

	/**
	 * If command is invalid, return 0.
	 */
	ClientPacket * pack( ClientCommandBase * cmd );


	/**
	 * if classes don't match, return 0.
	 */
	ClientPacket * pack( QList<ClientCmmandBase*> cmdList );

	/**
	 *
	 */
	bool cmdHeartBeat( qint64 ackID=0 );

	/**
	 *
	 */
	bool cmdHandShake();

private slots:

	/**
	 *
	 */
	void ackHeartBeat( ClientCommandHeartBeat* hb );

private:
	connectionState state;
	qint64 heartBeatCount;
};

}	//QtuC::
#endif //CLIENTCONNECTIONMANAGERBASE_H

