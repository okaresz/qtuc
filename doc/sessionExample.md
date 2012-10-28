Session Example		{#doc-session_example}
==========================

Configuration		{#doc-session_example-conf}
-------------------------------

  1. **Defining the device API**: First you have to plan the structure of your "device state". You must define de hardware interfaces, state variables and device functions in the [deviceAPI config file](@ref doc-deviceAPIxml). You have to have at least one hardware interface with a variable.
  2. **Check qcProxy settings**: By the qcProxy executable, there is an xml file named *qcProxySettings.xml*. (If not, start the proxy, and close it, the settings will be generated automatically.) Open and edit it to suit your environment. (And I mean most of all the network!)
  3. **Placing the files**: Copy the qcProxy executable, the *deviceAPI.xml* and *qcProxySettings.xml* under the same directory.
  4. **Check environment**: Ensure that the proxy can read/write the defined serial or other port, has the privilages to create and modify files in the working directory, and that the defined TCP port is open (check firewall if there's any) and routed to the current machine.
  5. **Start qcProxy**: If everithing seems OK, start the proxy. If not, proxy will tell you anyway. At this point you have a working proxy, with hopefully a working connection to the device. And if you've implemented the command interface on the device according to the documentation, proxy communicating and is syncing the state already. Yay!
  6. **GUI**: If you would like to use qcGUI to monitor and control the device state on a GUI, read further.
  7. **Check qcGUI settings**: By the qcGUI executable, there is an xml file named *qcGUISettings.xml*. (If not, start the gui, and close it, the settings will be generated automatically.) Open and edit it to suit your environment. (And I mean most of all the network!)
  8. **Placing the files**: Copy the qcGUI executable and *qcGUISettings.xml* under the same directory.
  9. **Check environment**: Ensure that the gui has the privilages to create and modify files in the working directory, and that the defined TCP port is open (check firewall if there's any) and routed to the current machine.
  10. **Start qcGUI**: If everithing seems OK, start the GUI. The GUI should appear with some basic tools. Find the Connect button to connect to the proxy. If you click it, and you've set everything well, the GUI should connect, and display all the available widgets to use your device. If there's no GUI at all, something went terribli wrong...

Initialization sequence		{#doc-session_example-init_seq}
-------------------------------

You only need this part, if you are a client / proxy developer. For everyday use, the [configuration](@ref doc-session_example-conf) part is sufficient.

As the starting order of the components are unknown, the init sequence must be defined accordingly.

### Proxy <-> device ###

  1. After the device is powered on, it has no obligation, just to listen on the communication port for commands.
  2. When the proxy starts, first it reads the settings and the device API. If any of this fails, exits with an error message.
  3. The proxy begins to find the device and sends out a handshake request on the defined port to the device. The device must reply with a valid handshake acknowledge message.
  4. After a successful handshake, the proxy will sync all the variables defined in the deviceAPI (send a get command for each of them), to wich the device must reply with the right set commands.
  5. The proxy sets up the autoUpdates according to the deviceAPI file, and starts the update loop.

### Proxy <-> client ###

A client can be any compatible application which complies with the clientCommand syntax, and connects to the proxy through a TCP socket.

  1. After opening the TCP socket, first the client must send a handshake message with its own introduction (name, description, etc...), see [clientProtocol page](@ref doc-clientProtocol-packets-handshake) for details. To this, proxy will reply with a [proxy handshake packet](@ref doc-clientProtocol-packets-handshake).
  2. When the client is ready to receive the deviceAPI, it must send the [deviceAPI request](@ref doc-clientProtocol-packets-deviceAPI_req). The proxy will reply with the [deviceAPI](@ref doc-clientProtocol-packets-deviceAPI) packet.
  3. After this, it's the responsibility of the client what to do with the API, and how to handle it. Usually, first a synchronization is recommended to display an up-to-date state of the device. This can be achieved with a [device packet](@ref doc-clientProtocol-packets-device), uing the `<get/>` command.
  4. Also, if the deviceAPI contains state variables which must be auto-updated on the client side, a [variable subscription](@ref doc-clientProtocol-packets-subscribe) should be sent to the proxy. If the subscription succeeds, a [device](@ref doc-clientProtocol-packets-device) packet with a set command will be sent periodically to the client with the requested variable.
  5. The client must send a [HeartBeat](@ref doc-clientProtocol-packets-heartbeat) packet every seconf to the proxy. If The heartBeats are missing, the connection is considered to be lost.
