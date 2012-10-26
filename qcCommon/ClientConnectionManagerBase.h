#ifndef CLIENTCONNECTIONMANAGERBASE_H
#define CLIENTCONNECTIONMANAGERBASE_H

#include "ErrorHandlerBase.h"
#include <QTcpSocket>
#include "ClientCommandFactory.h"

namespace QtuC
{

enum connectionState_t
{
	connectionUndefined,
	connectionUnInitialized,
	connectionConnected,
	connectionHandShaking,
	connectionReady,
	connectionLost,
	connectionDisconnected,
	connectionError
};

class ClientCommandBase;

/** Class ClientConnectionManagerBase.
 * Provides a basic interface to communicate with a client.
 * Specific Client connection manager classes should inherit this.*/
class ClientConnectionManagerBase : public ErrorHandlerBase
{
	Q_OBJECT
public:
	ClientConnectionManagerBase( QTcpSocket *socket, QObject *parent = 0 );

	void commandReceived( ClientCommandBase cmd );

	/**
	 * Commands should be created with new on the heap.
	 */
	bool sendCommand( ClientCommandBase * cmd );

	/**
	 * Send multiple commands of the same class in one packet.
	 */
	bool sendCommands( QList<ClientCommandBase*> cmdList );

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
	static qint64 mPacketCount;
	static QHash<QString,QString> mSelfInfo;
	QHash<QString,QString> mClientInfo;
	QTcpSocket* mClientSocket;
	ClientCommandFactory *mCommandFactory;

	/**
	 * If command is invalid, return 0.
	 */
	ClientPacket * pack( ClientCommandBase * cmd );


	/**
	 * if classes don't match, return 0.
	 */
	ClientPacket * pack( QList<ClientCommandBase*> cmdList );

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
	connectionState_t mState;
	qint64 mHeartBeatCount;
};

}	//QtuC::
#endif //CLIENTCONNECTIONMANAGERBASE_H

