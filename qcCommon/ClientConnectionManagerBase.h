#ifndef CLIENTCONNECTIONMANAGERBASE_H
#define CLIENTCONNECTIONMANAGERBASE_H

#include "ErrorHandlerBase.h"
#include <QTcpSocket>
#include "ClientCommandFactory.h"
#include "ClientPacket.h"

namespace QtuC
{

/** Connection states.
  *  * <b>connectionUndefined</b>: State is undefined.
  *  * <b>connectionUnInitialized</b>: Socket is not opened yet.
  *  * <b>connectionConnected</b>: Socket is opened, but no communication yet.
  *  * <b>connectionHandShaking</b>: In the handShaking process.
  *  * <b>connectionReady</b>: HandShake done, client is connected and calid, ready for commands.
  *  * <b>connectionLost</b>: Connection is lost. The connection is considered lost if no heartBeat is received for 3 seconds.This timeout can be changed in settings.
  *  * <b>connectionDisconnected</b>: The connection was closed. (Either deliberately or because of an error.)
  *  * <b>connectionError</b>: En error occured on the connection, usually meaning that normal communication is impossible.*/
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

/** ClientConnectionManagerBase class.
 *	Provides a basic interface to communicate with a client.
 *	@todo more detailed doc, howto use, and about deleting packets and commands,,, (responsibility of higher level code, ClientConnectionManagerBase won't delete them)
 *	Specific client connection manager classes should inherit this.*/
class ClientConnectionManagerBase : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** Constructor.
	 *	@param socket The TCP socket of the client.*/
	ClientConnectionManagerBase( QTcpSocket *socket, QObject *parent = 0 );

	/** Get whether the client is connected.
	  *	For more detailed information, call getState().
	  *	@return True if connection state is connectionConnected, connectionHandShaking or connectionReady, false otherwise.*/
	bool isConnected() const;

	/** Get whether the client is connected and ready.
	  *	For more detailed information, call getState().
	  *	@return True if connection state is connectionReady, false otherwise.*/
	bool isReady() const;

	/** Get the client's ID string.
	 *	@return The client's ID string.*/
	const QString &getID() const;

	/** Get connection state.
	 *	@return Connection state.*/
	connectionState_t getState() const;

	/** Get client info.
	 *	Return stored clientinfo, sent by client in the handShake command.
	 *	@return Client information as a QHash.*/
	const QHash<QString,QString> getClientInfo() const;

	/** Get self info list.
	 *	Return server information list, to be sent to the client during handShake.
	 *	@return Server information list as a QHash.*/
	const QHash<QString,QString> getSelfInfo() const;

	/** Get self info.
	 *	Return server information with the key infoName.
	 *	@return Server information with the key infoName.*/
	const QString getSelfInfo( const QString &infoName) const;

	/** Set an advertised self-information.
	 *	To set more info at a time, see setSelfInfo( const QHash<QString,QString>& ).
	 *	@param key Name of the info.
	 *	@param value Info value.*/
	void setSelfInfo( const QString &key, const QString &value );

	/** Set the advertised self-information.
	  *	@warning This must be set before the handshake. Calling after will do nothing.
	  *	@param The information lis to set.*/
	bool setSelfInfo( const QHash<QString,QString> &infoList );

	/** Send a command to the client.
	 *  Grab the command, wrap it into a packet and send it.
	 *	Commands should be created with new on the heap.
	 *	@param cmd The command to send.
	 *	@return True on success, false otherwise.*/
	bool sendCommand( ClientCommandBase *cmd );

	/** Send multiple commands to the client.
	 *	The commands will be sent in one packet.
	 *	If a command is invalid, it will be discarded.
	 *  Only commands of the same class can be sent together! The class of the packet will be the packetClass of the first command. If a class of a subsequent command differs, it will be discarded.
	 *	Commands should be created with new on the heap.
	 *	@param cmdList List of the commands to send.
	 *	@return True on success, false otherwise.*/
	bool sendCommands( const QList<ClientCommandBase*> &cmdList );

	/** Send a packet.
	  *	@param packet The packet to send.
	  *	@return True on success, false otherwise.*/
	bool sendPacket( ClientPacket *packet );

signals:

	/** Emitted when a packet is received from the client.
	  * The QObject parent of the packet is the ClientConnectionManager, that created the packet, so the packet will be deleted by the destructor of connection manager,
	  * but the packet must be destroyed as soon as it's handled to free memory.
	  *	@param clientPacket Pointer to the client packet object.*/
	void packetReceived( ClientPacket *clientPacket );

	/** Emitted when a command is received from the client.
	  * The QObject parent of the command is the packet it belongs to, so the command will be deleted by the destructor of the packet,
	  *	but the command must be destroyed as soon as it's handled, to free memory.
	  *	@param clientCommand The client command object. The object is the instance of the Client command class corresponding to the command.*/
	void commandReceived( ClientCommandBase *clientCommand );

protected:

	connectionState_t mState;	///< Connection state.
	qint64 mHeartBeatCount;		///< heartBeat count.
	static QHash<QString,QString> mSelfInfo;	///< Self-information to send to the client during handShake.
	QHash<QString,QString> mClientInfo;		///< Client information sent by the client during handShake.
	QTcpSocket* mClientSocket;	///< TCP socket for the client connection.
	ClientCommandFactory *mCommandFactory;	///< A ClientCommandFactory instance to build and initialize client commands.

protected slots:

	/** Handle new Client data.
	  *	This slot is connected to QTcpSocket::readyRead().*/
	void receiveClientData();

	/** Handle received ClientPacket.
	  *	This slot is connected to packetReceived() signal.
	  *	@param The packet received.*/
	void handleReceivedPacket( ClientPacket *packet );

	/** Reflex.
	  *	Reflexes are instant actions to an incoming packet/command.
	  *	this slot is connected to commandReceived(), and handles reflexes.*/
	void reflex( ClientCommandBase *command );

	/** Reply to a heartBeat command.
	 *	This will construct an acknowledge heartBeat in reply for the passed command.
	 *	@param incomingHeartBeat A heartBeat command sent by the client.*/
	void replyHeartBeat( ClientCommandHeartBeat* incomingHeartBeat );

	/** Acknowledge and reply to a handShake command.
	 *	This will set client information based on the received handShake, and construct a handShake in reply.
	 *	@param handShake The handShake command sent by the client.*/
	void ackHandShake( ClientCommandHandShake *handShake );

private:

	/** Check client socket.
	  *	Check if it is open, readable and writable.
	  *	@return True if socket is open, readable and writable, false if not.*/
	bool checkSocket();

};

}	//QtuC::
#endif //CLIENTCONNECTIONMANAGERBASE_H

