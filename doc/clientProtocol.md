Client Protocol		{#doc-clientProtocol}
========================

The proxy can communicate with the clients (qcGUI or any other 3rd party client) through a custom XML-based protocol.
The protocol is packet based. A raw packet (on the transport layer) has the following structure:

  * First a 16bit unsigned integer in network endianness (big-endian), holding the size of the following XML data in bytes.
  * Immediately after the size, an UTF-8 encoded text stream begins, without BOM. This stream is the XML data of the clientProtocol.

The packets are parsed and processed on arrival, and the bare XML data remains (software layer).
From this point, a "packet" means only the processed, decoded XML data.
  
## Packets ## {#doc-clientProtocol-packets}

The packets are grouping the similar commands/data as a container:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="<packetClass>" id="<packetID>" re="<replyTo_packetID>">
	...
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **class**: The packet class. There are two type of packets so far:
    * *device*: These packets contain a deviceCommand, and are used to get/set a state variable or call a device function.
    * *control*: These packets are designated for the proxy or the client and can contain various control commands like `quit` or `heartBeat`, etc...
  * **id**: The packet ID. This is a string, consisting of the ID string (see [Handshake command](#doc-clientProtocol-packets-handshake)) of the sender, and the packet number, separated by a hashmark. The packet number is an integer number, incremented on every packet, starting with 1. An example would be: `qcProxy#265` meaning this is a 265th packet sent by the qcProxy.
  * **re**: There are several packets, which are sent in reply to another (a set device command to a get request, a heartBeat ACK, or a deviceAPI packet to a deviceAPI request), in this case the ack attribute holds the original packet ID to which this packet is a reply (or ACK).

If you want to send more commands which belong to the same packet class, you can send them in one packet. In that case, the commands are processed in order.

### Device packets ###		{#doc-clientProtocol-packets-device}

A device packet is essentially a device command. Three example for the three types of deviceCommand:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="device" id="clientID#2328">
	<get hwi="led" var="dY"/>
</packet>

<packet class="device" id="qcProxy#2331" re="clientID#2328">
	<set hwi="led" var="dY"><![CDATA[on]]></set>
</packet>

<packet class="device" id="clientID#2367">
	<call hwi="LCD" func="write"><![CDATA[Hi LCD!]]></call>
	<call hwi="drive" func="eStop"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The argument must always be wrapped in a CDATA node.

Special cases of commands are possible for requesting several variables at a time. To get all variable in a hardware interface, send `<get hwi="hwI_name"/>` in a device packet. Or if you want to get ALL the variables (don't do this very often though... Use [subscribe](#doc-clientProtocol-packets-subscribe) instead.), send `<get/>`.


### Control packets ###		{#doc-clientProtocol-packets-control}

There are several types of control packets, and the list is not yet complete... Feel free to implement more.

#### Handshake ####		{#doc-clientProtocol-packets-handshake}

The handshake packet is used as the first packet sent from the client to the proxy, and from the proxy to the client in return. An example handshake would be:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#2338">
	<handshake from="client">
		<id>clientID</id>
		<name>Client name</name>
		<desc>A more or less detailed description of the client.</desc>
		... <!-- custom nodes allowed -->
	</handshake>
</packet>

<packet class="control" id="qcProxy#2339" re="clientID#2338">
	<handshake from="proxy">
		<id>qcProxy</id>
		<name>QtuC Proxy</name>
		<desc>desc...</desc>
		<author>Károly Oláh</author>
		<version>1.3.4</version>
	</handshake>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **id**: The ID string of the client. A unique identifier, specific to this client. Can only contain letters from the english alphabet, numbers, and the underscore (regex: `[a-zA-Z0-9_]`), case sensitive.
  * **name**: A client name of your choice.
  * **desc**: A description of the client.


#### HeartBeat ####		{#doc-clientProtocol-packets-heartbeat}

A heartbeat command must be sent by the client every second (1Hz). The proxy may estimate connection speed and status from the periodic heartbeats. If the heartbeats are missing, the connection considered to be lost.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#334">
	<heartBeat id="1445"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

For every HeartBeat, the proxy will reply with an acknowledge HeartBeat:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="qcProxy#338">
	<heartBeat ack="1445"/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#### deviceAPI request ####		{#doc-clientProtocol-packets-deviceAPI_req}

When the client is ready to handle the deviceAPI definition, it sends the deviceAPI request package:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#4">
	<reqDeviceAPI/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#### deviceAPI ####		{#doc-clientProtocol-packets-deviceAPI}

In this package, the whole deviceAPI definition is sent either from the client to the proxy, or back.

The proxy may send a deviceAPI pocket at any time throughout the session (or to a [deviceAPI request](#doc-clientProtocol-packets-deviceAPI_req)), meaning that the device interface has changed, and so the deviceAPI must be updated.
When a client receives this packet, it must take the necessary actions to accomodate the changes in the deviceAPI.

If the client sends a complete deviceAPI to the proxy, the proxy will update the device interface at runtime.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="qcProxy#6" re="clientID#4">
	<deviceAPI permanent="true" hash="hashofapidata"><![CDATA[base64(deviceAPI)]]></deviceAPI>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **permanent**: True or false. If false, the changes will take place but only for the current session, until next update or application exit. If true, the deviceAPI.xml file iss overwritten wit the new API.
  * **hash**: The md5 hash of the base64 encoded deviceAPI data which is sent in this packet. It's recommended to check the data against the hash...

The whole root node (with the root node tags included) of the deviceAPI file is sent, as a base64 encoded UTF-8 string, wrapped in a CDATA node.

The base64 encoding is done with QByteArray::toBase64(), so according to the Qt 4.8 manual, the algorithm used to encode Base64-encoded data is defined in [RFC 2045](http://www.rfc-editor.org/rfc/rfc2045.txt).


#### Subscribe ####		{#doc-clientProtocol-packets-subscribe}

The client can subscribe to a variable autoUpdate. After the subscription, the proxy will send the requested variable through a set command in a device packet with the defined frequency.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#23">
	<subscribe freq="5" hwi="led" var="dY">
	<subscribe freq="5" hwi="drive"/>
	<subscribe freq="5"/>
<packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

In this example, all three possible variations are demonstrated (not considering it's practical use)

  * **hwi**: A valid hardware interface name. Cannot be omitted, unless both `hwi` and `var` is omitted. In that case, ALL variable will be subscribed (last case).
  * **var**: A valid variable name in the given hardware interface. If omitted, all variable in the hardware integerface will be subscribed.
  * **freq**: Frequency of the updates in Hz (x per second).


#### unSubscribe ####		{#doc-clientProtocol-packets-unSubscribe}

In a similar fashion to subscribe, you can unsubscribe. After this command, proxy will stop sending the update packets immediately.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#73838">
	<unSubscribe hwi="led" var="dY">
	<unSubscribe hwi="drive"/>
	<unSubscribe/>
<packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

If an unsubscribe command is sent for a variable that isn't subscribed, the command is silently ignored. This is the case for subscribe commands as well.


#### Message ####		{#doc-clientProtocol-packets-message}

Messages can be sent to the client with the message commands. This messages are either from the device, routed to the client by the proxy, or from the proxy itself. Sending messages to the proxy has little practical use, they are logged, though.

A message from the proxy:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="qcProxy#524">
	<message level="<msgLevel>"><![CDATA[<message text>]]></message>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  * **level**: Level of the message. Possible values:
    * *debug*: A debug message.
    * *info*: Some information.
    * *warning*: A non-fatal warning message.
    * *error*: A (in most cases) non-fatal error.
    * *critical*: A critical error, likely to result in an unknown critical state or application shutdown.


#### Re-read deviceAPI ####		{#doc-clientProtocol-packets-reReadDeviceAPI}

When the proxy receives this package, it will re-read the deviceAPI file, and reinitialize itself, considering the changes. After this command, all autoUpdates will stop.

The client must take the necessary actions to adapt to the possibly changed deviceAPI. After sending this command, the proxy parses the new API definition and if it's valid, reconfigures the device interface. When the changes are done, the proxy will send out the new deviceAPI to all clients with the [deviceAIP](#doc-clientProtocol-packets-deviceAPI) packet.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#6367">
	<reReadDeviceAPI/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#### Disconnect ####		{#doc-clientProtocol-packets-disconnect}

When this packet is sent to the proxy, the proxy will stop all auto update and all data flow towards the client, disconnect the client, and close the socket.
(Keeping the device-proxy connection alive.)

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#78346">
	<disconnect/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#### Quit ####		{#doc-clientProtocol-packets-quit}

When this packet is sent to the proxy, the proxy will stop all auto update and all data flow, disconnect all clients, and the device and will exit gracefully,

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<packet class="control" id="clientID#78347">
	<quit/>
</packet>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
