Client Protocol		{#doc-clientProtocol}
=====================

The proxy communicates with the clients (qcGUI or any other 3rd party client) with a custom XML-based protocol.
The protocol is packet based. A raw packet (on the transport layer) has the following structure:

  * First a 16bit unsigned integer in network endianness (big-endian), holding the size of the following XML data in bytes.
  * Immediately after the size, an UTF-8 encoded text stream begins, without BOM. This stream is the XML data of the clientProtocol.

The packets are parsed and processed on arrival, and the bare XML data remains (software layer).
From this point, a "packet" means only the processed, decoded XML data.
  
# Packets # {#doc-clientProtocol-packets}

The packet wraps a client command. The purpose of the packet is to carry the parameters and states of the protocol, identify the clients, optimize data flow, etc..
A packet can include several client commands, but ony from/to the same client.

The syntax of a packet:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="<packetID>" re="<replyTo_packetID>">
	...
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **id**: The packet ID, compulsory. This is a string, consisting of the ID string (see [Handshake command](#doc-clientProtocol-packets-handshake)) of the sender, and the packet number, separated by a hashmark. The packet number is an integer number, incremented on every packet, starting with 1. An example would be: `qcProxy#265` meaning this is a 265th packet sent by the qcProxy.
  * **re**: Optional. There are several packets, which are sent in reply to another (a set device command to a get request, a heartBeat ACK, or a deviceAPI packet to a deviceAPI request), in this case the ack attribute holds the original packet ID to which this packet is a reply (or ACK).

You can send multiple commands in one packet. In that case, the commands are processed in order.

# Client commands #		{#doc-clientProtocol-command}

There are two types of commands:
  * *device*: This command is used to get/set a state variable or call a device function.
  * *control*: These commands are designated for the proxy or the client and can mean for example a command like `quit` or `heartBeat`, etc...
  
These types don't appear in the protocol explicitly.
  
## Device command ##		{#doc-clientProtocol-command-device}

A client-side device command (`ClientCommandDevice`) is essentially a bare device command. Three example for the three types of deviceCommand:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#2328">
	<get hwi="led" var="dY"/>
</packet>

<packet id="qcProxy#2331" re="clientID#2328">
	<set time="4f6da8" hwi="led" var="dY"><![CDATA[on]]></set>
</packet>

<packet id="clientID#2367">
	<call hwi="LCD" func="write">
		<arg><![CDATA[0,0]]</arg>
		<arg><![CDATA[Hi LCD!]]></arg>
	</call>
	<call hwi="drive" func="eStop"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**time**: The `time` attribute is the *last modified* or creation timestamp of the variable as a standard UNIX timestamp as a hexadecimal number, without the `0x` prefix.
`time` only makes sense in a set command, so it's only included in a set command.
If device uses timekeeping and sends timestamps in all commands, this timestamp will be exactly the time when the value was created on the device.
Id device doesn't send a timestamp, proxy uses the time the variable is updated from the incoming device command. See [timekeeping](@ref mainpage-concept-timekeeping) for more details.

**arg**: The arguments must always be wrapped in a CDATA node.

Special cases of commands are possible for requesting several variables at a time. To get all variable in a hardware interface, send `<get hwi="hwI_name"/>`. Or if you want to get ALL the variables (don't do this very often though... Use [subscribe](#doc-clientProtocol-packets-subscribe) instead.), send `<get/>`.


## Control commands ##		{#doc-clientProtocol-command-control}

There are several types of control packets, and the list is not yet complete... Feel free to implement more.

### Handshake ###		{#doc-clientProtocol-command-control-handshake}

The handshake packet is used as the first packet sent from the client to the proxy, and from the proxy to the client in return.
The client must then acknowledge the server's reply with an empty handShake.
An example handshake would be:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#2338">
	<handshake from="client">
		<id>clientID</id>
		<name>Client name</name>
		<desc>A more or less detailed description of the client.</desc>
		... <!-- custom nodes allowed -->
	</handshake>
</packet>

<packet id="qcProxy#234" re="clientID#2338">
	<handshake from="proxy" ack="true">
		<id>qcProxy</id>
		<name>QtuC Proxy</name>
		<desc>desc...</desc>
		<author>okaresz</author>
		<version>1.3.4</version>
	</handshake>
</packet>

packet id="clientID#2339" re="qcProxy#234">
	<handshake from="client" ack="true" />
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **id**: Compulsory. The ID string of the client. A unique identifier, specific to this client. Can only contain letters from the english alphabet, numbers, and the underscore (regex: `[a-zA-Z0-9_]`), case sensitive.
  * **name**: A client name of your choice, optional.
  * **desc**: A description of the client, optional.
  * **ack**: Whether the handShake was accepted. If the client is rejected, this will be false, and the connection is likely to be closed by the remote end.


### HeartBeat ###		{#doc-clientProtocol-command-control-heartbeat}

\warning Not implemented yet

A heartbeat command must be sent by the client every second (1Hz). The proxy may estimate connection speed and status from the periodic heartbeats. If the heartbeats are missing, the connection considered to be lost.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#334">
	<heartBeat id="1445"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For every HeartBeat, the proxy will reply with an acknowledge HeartBeat:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="qcProxy#338">
	<heartBeat ack="1445"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### deviceAPI request ###		{#doc-clientProtocol-command-control-deviceAPI_req}

When the client is ready to handle the deviceAPI definition, it sends the deviceAPI request package:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#4">
	<reqDeviceAPI/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### deviceAPI ###		{#doc-clientProtocol-command-control-deviceAPI}

In this package, the whole deviceAPI definition is sent either from the client to the proxy, or back.

The proxy may send a deviceAPI pocket at any time throughout the session (or to a [deviceAPI request](#doc-clientProtocol-packets-deviceAPI_req)), meaning that the device interface has changed, and so the deviceAPI must be updated.
When a client receives this packet, it must take the necessary actions to accomodate the changes in the deviceAPI.

If the client sends a complete deviceAPI to the proxy, the proxy will update the device interface at runtime.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="qcProxy#6" re="clientID#4">
	<deviceAPI permanent="true" hash="hashofapidata"><![CDATA[base64(deviceAPI)]]></deviceAPI>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **permanent** (not implemented): True or false. If false, the changes will take place but only for the current session, until next update or application exit. If true, the deviceAPI.xml file iss overwritten wit the new API.
  * **hash**: The md5 hash of the base64 encoded deviceAPI data which is sent in this packet. It's recommended to check the data against the hash...

The whole root node (with the root node tags included) of the deviceAPI file is sent, as a base64 encoded UTF-8 string, wrapped in a CDATA node.

The base64 encoding is done with QByteArray::toBase64(), so according to the Qt 4.8 manual, the algorithm used to encode Base64-encoded data is defined in [RFC 2045](http://www.rfc-editor.org/rfc/rfc2045.txt).


### DeviceInfo ###		{#doc-clientProtocol-command-control-deviceInfo}

The deviceInfo command provides information about the device. This command is sent by the proxy to a client.

The parameters in this command are the same as in the [similar section](@ref doc-deviceAPIxml-deviceInfo) of the DeviceAPI, and the [device greeting message](@ref doc-deviceCommand-special-greeting).

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="qcProxy#6">
	<deviceInfo startup="false" startupTime="32f6e6a7182361817">
		<info name="positiveAck">false</info>
		<info name="name">deviceName</info>
		<info name="desc">A short description of the device</info>
		<info name="platform">A short platform string, eg.: STM32F4</info>
		<info name="project">projName</info>
	</deviceInfo>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **startup**: True means that this command is sent by the proxy because the device has just started up (whether it had sent a greeting or not, this client command is sent by the proxy). If false or absent, than this is a reply to a client request.
  * **startupTime**: The Time of the device startup, as a UNIX timestamp expressed in milliseconds, in base 16 (hexa), without prefix. This is always a valid, non-zero timestamp. See [timekeeping](@ref mainpage-concept-timekeeping).

The child nodes are the device parameters. Only those params are included, which the client has privilege to see.


### Request deviceInfo ###		{#doc-clientProtocol-command-control-reqDeviceInfo}

The client can send a deviceInfo request to the proxi, to which the proxy must reply with a deviceInfo command.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#8">
	<reqDeviceInfo/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Subscribe ###		{#doc-clientProtocol-command-control-subscribe}

The client can subscribe to a variable autoUpdate. After the subscription, the proxy will send the requested variable with a set command in a device packet at the defined intervals.
The interval is a 32bit integer, interpreted as milliseconds.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#23">
	<subscribe interval="5" hwInterface="led" variable="dY">
	<subscribe interval="5" hwInterface="drive"/>
	<subscribe interval="5"/>
<packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example, all three possible variations are demonstrated (not considering it's practical use)

  * **hwi**: A valid hardware interface name. Cannot be omitted, unless both `hwi` and `var` is omitted. In that case, ALL variable will be subscribed (last case).
  * **var**: A valid variable name in the given hardware interface. If omitted, all variable in the hardware integerface will be subscribed.
  * **interval**: 32bit integer, interpreted as milliseconds, the time between two updates.

You cannot subscribe for the same set of variables twice. Two subscriptions are considered the same if both the hardwre interface and the variable is the same. Interval may be different.

If you would like to change the interval of a subscription, you must first unsubscribe the previous, then subscribe again with the new interval.

As a result of the subscribe request, the proxy will send a subscription feed periodically, at the requested intervals. This feed is a ClientPacket, including a ClientCommandDevice *set* command for each variable included in the subscription.

**Important!** A deviceVariable is always sent with the most specific subscription, including that variable (independent of the interval).
Subscription "A" is more specific than subscription "B" if "A" corresponds to a smaller, more specific set of variables.
For example a subscription for a hardware interface is more specific than a subscription for all the variables (in all interfaces).
This way, the most specific subscription is, of course, a subscription to a single variable.

*Example*: If there is a subscription of 1000ms interval for the hardwareInterface *"drive"*, and another with 100ms for *motorSpeed* in the *drive* interface, then proxy will send a subscription feed package with the *motorSpeed* variable every 100ms,
and a package with all the variables (as device commands) in the *drive* interface **except** *motorSpeed*, every second.

**An uninitialized deviceVariable is not updated**. So if a deviceVariable has never been set from the device, (it's Null), it won't be included in the subscription update feed.

If the deviceAPI.xm contains a valid user-side [autoUpdate node](@ref doc-deviceAPIxml-stateVarList) for a variable, it is up to the client whether it uses this information to automatically send a subscription to the proxy for that variable.


### unSubscribe ###		{#doc-clientProtocol-command-control-unSubscribe}

In a similar fashion to subscribe, you can unsubscribe. After this command, proxy will stop sending the update packets immediately.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#73838">
	<unSubscribe hwInterface="led" variable="dY">
	<unSubscribe hwInterface="drive"/>
	<unSubscribe/>
<packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If an unsubscribe command is sent for a variable that isn't subscribed, the command is silently ignored.

A subscription can only be cancelled by an unsubscribe command with the **exact same parameters** (excluding interval). However, by setting the variable or the interface to "*" (asterisk), a bulk cancel can be requested.

If *hwi* and *var* both equal "*", all subscriptions are cancelled.
If *hwi* is a valid hardware interface name, and *var* is "*", then all subscriptions for the interface or a variable in the interface is cancelled.


### Message ###		{#doc-clientProtocol-command-control-message}

\warning Not implemented yet

Messages can be sent to the client with the message commands. This messages are either from the device, routed to the client by the proxy, or from the proxy itself. Sending messages to the proxy has little practical use, currently the proxy discards them.

A message from the proxy or the device:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="qcProxy#524">
	<message sender="<sender>" level="<msgLevel>"><![CDATA[<message text>]]></message>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **sender**: Sender of the message
    * *proxy*: The sender is the proxy.
    * *device*: The sender is the device.
  * **level**: Level of the message. Possible values:
    * *debug*: A debug message.
    * *info*: Some information.
    * *warning*: A non-fatal warning message.
    * *error*: A (in most cases) non-fatal error.
    * *critical*: A critical error, likely to result in an unknown critical state or application shutdown.

### Status ###		{#doc-clientProtocol-command-control-status}

\warning Not implemented yet

This is a one-direction command, only the proxy can send it to clients, to notify them of the current device and proxy status.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="qcProxy#554">
	<status of="<source>" about="<message>"><![CDATA[<status>]]></status>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **source**: Source of the status
    * *device*: Status of the device
    * *proxy*: Status of the proxy
  * **message**: A short description of the status change.
  * **status**: status info. can be:
    * *init*:	Initializing.
    * *configure*: (re)configure/change. Usually means a temporary pause in communication.
    * *ready*: Everithing is ok and usable.
    * *error*: In error state, communication stopped.
    
    
### Re-read deviceAPI ###		{#doc-clientProtocol-packets-control-reReadDeviceAPI}

\warning Not implemented yet

When the proxy receives this package, it will re-read the deviceAPI file, and reinitialize itself, considering the changes. After this command, all autoUpdates will stop.

The client must take the necessary actions to adapt to the possibly changed deviceAPI. After sending this command, the proxy parses the new API definition and if it's valid, reconfigures the device interface. When the changes are done, the proxy will send out the new deviceAPI to all clients with the [deviceAPI](#doc-clientProtocol-packets-deviceAPI) packet.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#6367">
	<reReadDeviceAPI/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Disconnect ###		{#doc-clientProtocol-packets-control-disconnect}

\warning Not implemented yet

When this packet is sent to the proxy, the proxy will stop all auto update and all data flow towards the client, disconnect the client, and close the socket.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#78346">
	<disconnect/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


### Quit ###		{#doc-clientProtocol-packets-control-quit}

\warning Not implemented yet

When this packet is sent to the proxy, the proxy will stop all auto update and all data flow, disconnect all clients, and the device and will exit gracefully,

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet id="clientID#78347">
	<quit/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
