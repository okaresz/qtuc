#ifndef CLIENTPACKET_H
#define CLIENTPACKET_H

#include "ErrorHandlerBase.h"
#include <QDomElement>
#include "ClientCommandFactory.h"

namespace QtuC
{

class ClientConnectionManagerBase;

/** ClientPacket class.
 *	@todo more doc... must specify parent connection manager object, but be sure to delete every packet after sending it.
 * Represents a client packet, for encapsulating clientCommands of the same class.*/
class ClientPacket : public ErrorHandlerBase
{
	Q_OBJECT
public:

	/** An empty constructor.
	 * Use when you want to build a packet from scratch.*/
	ClientPacket(  );

	/** Wrap a client command with a packet.
	  *	@param clientCommand The client command tho wrap in a packet.*/
	ClientPacket( ClientCommandBase *clientCommand, QObject *parent = 0 );

	/** Destroy packet.*/
	~ClientPacket();

	/** Check if this packet is valid.
	  *	@return True if packet is valid, false otherwise.*/
	bool isValid();

	/** Get packet count.
	  *	Count of all the packet that was instantiated from this class.
	  *	@return Packet count.*/
	static quint64 getPacketCount()
		{ return mPacketCount; }

	/** Get the ID number of the packet.
	  *	@return The sequential number of the packet.*/
	quint64 getIDNumber() const;

	/** Get the ID string of the packet.
	  *	Format of the ID string is <serverShortName>#<packetIdNum>.
	  *	@return The ID string of the packet.*/
	const QString getID() const;

	/** Get the packet ID to which this apcket is a reply.
	  *	@return The reply-to packet id string.*/
	const QString getReplyTo() const;

	/** Get packet class.
	  *	@todo Solve class chaos.
	  *	@return packet class.*/
	//packetClass_t getClass() const;

	/** Get the list of commands within this pocket.
	  *	@return The lis of ClientCommands. May be empty if packet is invalid.*/
	const QList<ClientCommandBase*> getCommands();

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
	static quint16 readPacketSize( const QByteArray &rawData );

	static void setCommandFactoryPtr( ClientCommandFactory *factoryPtr )
		{ mCommandFactoryPtr = factoryPtr; }

	/** Set self-id.
	  *	The short name part of the packet id.
	  *	@param newId The new selfId.
	  *	@return True on success, false otherwise.*/
	static bool setSelfId( const QString &newId );

	/** Set the packet ID to which this packet is a reply.
	  *	@param replyToID The reply-to packet id string.*/
	void setReplyTo( const QString & replyToID );

	/** Set the packet to which this packet is a reply.
	  *	@param replyToPacket Reply to this packet.*/
	void setReplyTo( const ClientPacket &replyToPacket );

	/** Set the class of the packet.
	 *	You must set the class before appending any command. After a command has been appended, the packet class can not be set or changed.
	 *	The packet class must be set before appending any commands.
	 *	@param	True on success, false otherwise.*/
	//bool setClass( packetClass_t pClass );

	/** Append a client command.
	 *	Only client commands which are of the right class can be appended. (The class of the packet.)
	 *	The packet class must be set before appending any commands.
	 *	@return True on success, false otherwise.*/
	bool appendCommand( ClientCommandBase *clientCommand );

	/** Detach and return a command.
	  * This will delete command from the packet.
	  *	@param index Index of the command in the command list.
	  *	@return The detached client command object.*/
	ClientCommandBase *detachCommand( int index );

	/** Detach and return a command.
	  * This will delete command from the packet.
	  *	@param command The command in the command list.
	  *	@return The detached client command object, if found, 0 if not.
	  * @warning This function may return null, watch for deletions on a nullpointer, always check return value!*/
	ClientCommandBase *detachCommand( ClientCommandBase *command );

	/** Delete packet without deleting commands within the packet.
	  *	useful if you want to work with the commands later on, but the packet can be deleted*/
	void destroyShell();

private:

	/** Build a packet from raw packet data (use fromPacketData()!).
	 *	@note A new ID will be generated to the packet, the id attribute in packetElement is discarded.
	 *	@param packetElement The DOM element of the packet.
	 *	@return The ClientPacket object built from the element on success, or 0 on failure.*/
	ClientPacket( const QDomElement &packetElement, QObject *parent = 0 );

	/** Build the XML markup of the packet.
	  *	@return The packet element as a QDomElement of 0 on failure.*/
	QDomDocument *buildMarkup() const;

	/** Remove command from the command list.
	  *	@param index Index of the command in the command list.
	  *	@return Pointer to the removed command object.*/
	ClientCommandBase *removeCommand( int index );

	static quint64 mPacketCount;	///< Packet count, used for generating packet id.
	quint64 mIdNum;		///< Packet ID number. Initialized from packet count.
	QString mReplyTo;	///< Reply-to packet id.
	//packetClass_t mClass;		///< Class of the packet.
	static QString mSelfId;
	QList<ClientCommandBase*> mCmdList;		///< Commands to be included in the packet.
	static ClientCommandFactory *mCommandFactoryPtr;	///< Pointer to the ClientCommandFactory instance in the connection manager object of this packet.
};

}	//QtuC::
#endif //CLIENTPACKET_H

