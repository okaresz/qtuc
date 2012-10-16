#ifndef CLIENTPACKET_H
#define CLIENTPACKET_H

#include "ClientCommandDevice.h"

namespace QtuC
{

enum packetClass_t
{
	undefined_packet,
	control_packet,
	device_packet
};

/**
 * Class ClientPacket
 * Replresents a client packet.
 * For encapsulating clientCommands of the same class.*/
class ClientPacket
{
public:

	/**
	 * An empty constructor.
	 * Use when you want to build a packet from scratch.*/
	ClientPacket();
	ClientPacket( ClientCommandBase * clientCommand );

	int getIDNumber();

	QString getReplyTo();

	packetClass_t getClass();


	void setReplyTo( const QString & replyToID );

	/**
	 * Set the class of the packet.
	 * Set the class before appending any command. After a command has been appended, the Class can not be set.*/
	bool setClass( packetClass_t pClass );

	/**
	 * Append a command if it's in the right class.
	 */
	bool append( ClientCommandBase clientCommand );

	void getPacketData() const;

	static ClientPacket* fromPacketData( const QByteArray & rawPacket );

private:
	static  int idNum;
	replyTo QString;
	packetClass_t packetClass;
	QList<ClientCommandBase*> cmdList;

	/**
	 * Build a packet from raw packet data (use fromPacketData())
	 * Use when receiving a packet.*/
	ClientPacket( const QByteArray & rawPacket );
};

}	//QtuC::
#endif //CLIENTPACKET_H

