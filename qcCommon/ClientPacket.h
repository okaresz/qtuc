#ifndef CLIENTPACKET_H
#define CLIENTPACKET_H

#include "ErrorHandlerBase.h"
#include <QDomElement>

namespace QtuC
{

class ClientCommandBase;

/** Packet class type.
  *	 * <b>undefined</b>: Undefined, invalid packet.
  *  * <b>device</b>: These packets contain a deviceCommand, and are used to get/set a state variable or call a device function.
  *  * <b>control</b>: These packets are designated for the proxy or the client and can contain various control commands like quit or heartBeat, etc...*/
enum packetClass_t
{
	packetUndefined,
	packetControl,
	packetDevice
};

/** ClientPacket class.
 * Represents a client packet, for encapsulating clientCommands of the same class.*/
class ClientPacket : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** An empty constructor.
	 * Use when you want to build a packet from scratch.*/
	ClientPacket( QObject *parent = 0 );

	/** Wrap a client command with a packet.
	  *	@param clientCommand The client command tho wrap in a packet.*/
	ClientPacket( ClientCommandBase * clientCommand, QObject *parent = 0 );

	/** Get the number of the packet.
	  *	@return The sequential numer of the packet.*/
	int getIDNumber() const;

	/** Get the packet ID to which this apcket is a reply.
	  *	@return The reply-to packet id string.*/
	const QString getReplyTo() const;

	/** Get packet class.
	  *	@return packet class.*/
	packetClass_t getClass() const;

	/** Set the packet ID to which this apcket is a reply.
	  *	@param replyToID The reply-to packet id string.*/
	void setReplyTo( const QString & replyToID );

	/** Set the class of the packet.
	 *	You must set the class before appending any command. After a command has been appended, the packet class can not be set or changed.
	 *	The packet class must be set before appending any commands.
	 *	@param	True on success, false otherwise.*/
	bool setClass( packetClass_t pClass );

	/** Append a client command.
	 *	Only client commands which are of the right class can be appended. (The class of the packet.)
	 *	The packet class must be set before appending any commands.
	 *	@return True on success, false otherwise.*/
	bool append( ClientCommandBase *clientCommand );

	/** Get raw packet data, ready to send.
	 *	@return The raw packet.*/
	const QByteArray getPacketData() const;

	/** Build a ClientPacket from raw packet data.
	  *	@param rawPacket The raw packet data: an UTF-8 encoded text stream, without BOM, containing the XML node of the packet.
	  *	@return The ClientPacket object built from the data on success, or 0 on failure.*/
	static ClientPacket* fromPacketData( const QByteArray &rawPacket );

	/** Extract packet size from the beginning of a raw data stream.
	  *	Can be used to determine whether the whole packet has arrived yet.
	  *	@param rawData The raw data. Must be at least 4 byte long to read the packet size.
	  *	@return The packet size, converted to host endianness. If passed data was shorter than 4 bytes, return 0.*/
	static qint16 readPacketSize( const QByteArray &rawData ) const;

private:

	/** Build a packet from raw packet data (use fromPacketData()!).
	 *	@param packetData The raw packet data: an UTF-8 encoded text stream, without BOM, containing the XML node of the packet.
	 *	@return The ClientPacket object built from the data on success, or 0 on failure.*/
	ClientPacket( const QByteArray &packetData, QObject *parent = 0 );

	/** Build the XML markup of the packet.
	  *	@return The packet element as a QDomElement of 0 on failure.*/
	QDomDocument *buildMarkup();

	static int mPacketCount;	///< Packet count, used for generating packet id.
	int mIdNum;		///< Packet ID number. Initialized from packet count.
	QString mReplyTo;	///< Reply-to packet id.
	packetClass_t mClass;		///< Class of the packet.
	QList<ClientCommandBase*> mCmdList;		///< Commands to be included in the packet.
};

}	//QtuC::
#endif //CLIENTPACKET_H

