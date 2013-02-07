Session Example		{#doc-session_example}
==========================

# Configuration	 #		{#doc-session_example-conf}

  - **Defining the device API**: First you have to plan the structure of your "device state". You must define the hardware interfaces, state variables and device functions in the [deviceAPI config file](@ref doc-deviceAPIxml). You must have at least one hardware interface with a variable or a function.
  - **Check qcProxy settings**: Check and modify the QcProxy configuration file. See [Settings](@ref mainpage-settings) for more details.
  - **Check environment**: Ensure that the proxy can read/write the defined serial or other port, has the privileges to create and modify files in the working directory, and that the defined TCP port for QcProxy is open (check firewall if there's any) and routed to the current machine.
  - **Start qcProxy**: If everithing seems OK, start the proxy. If not, proxy will tell you anyway. At this point you have a working proxy, with hopefully a working connection to the device. And if you've implemented the command interface on the device according to the documentation, proxy is communicating and syncing the state already. Yay!
  - **GUI**: If you would like to use qcGUI to monitor and control the device state on a GUI, read further.
  - **Check qcGUI settings**: Similar to the proxy settings.
  - **Check environment**: Ensure that the gui has the privileges to create and modify files in the working directory, and that the defined TCP port is open (check firewall if there's any) for an outgoing connection.
  - **Start qcGUI**: If everithing seems OK, start the GUI. The GUI should appear with some basic tools. Click the *Connect* button to connect to the proxy. If you've set everything well, the GUI should connect, and display all the available widgets to use your device. If there's no GUI at all, something went terribly wrong...

# Initialization sequence #		{#doc-session_example-init_seq}

You only need this part, if you are a client / proxy developer. For everyday use, the [configuration](@ref doc-session_example-conf) part is sufficient.

As the starting order of the components are unknown, the init sequence must be defined accordingly.

## Proxy <-> device ##		{#doc-session_example-init_seq-proxy_device}

  - After the device is powered on, it should send a [device greeting message](@ref doc-deviceCommand-special-greeting). This is optional, but recommended.
  - When the proxy starts, first it reads the settings and the device API. If any of this fails, exits with an error message.
  - Not implemented yet! The proxy will sync all the variables defined in the deviceAPI (send a get command for each of them), to wich the device must reply with the right set commands.
  - The proxy sets up the autoUpdates according to the deviceAPI file, and starts the update loop.
  - Start listening on the TCP port set in the configuration file

## Proxy <-> client ##		{#doc-session_example-init_seq-proxy_client}

A client can be any compatible application which complies with the clientProtocol syntax, and connects to the proxy through a TCP socket.

  - After opening the TCP socket, first the client must send a handshake message with its own introduction (name, description, etc...), see [clientProtocol page](@ref doc-clientProtocol-command-control-handshake) for details. Proxy will reply with a [proxy handshake packet](@ref doc-clientProtocol-command-control-handshake). Finally the client must acknowledge the proxy reply with an ack handshake.
  - When the client is ready to receive the deviceAPI, it must send the [deviceAPI request](@ref doc-clientProtocol-command-control-deviceAPI_req). The proxy will reply with the [deviceAPI](@ref doc-clientProtocol-command-control-deviceAPI).
  - After this, it's the responsibility of the client what to do with the API, and how to handle it. Usually, first a synchronization is recommended to display an up-to-date state of the device. This can be achieved with a [device command](@ref doc-clientProtocol-command-device), uing the `<get/>`, or a [subscription](@ref doc-clientProtocol-command-control-subscribe).
  - Also, if the deviceAPI contains state variables which must be auto-updated on the client side, a [subscription](@ref doc-clientProtocol-command-control-subscribe) should be sent to the proxy. If the subscription succeeds, a *set* [device command](@ref doc-clientProtocol-command-device) will be sent periodically to the client with the requested variable.
  - (not implemented yet) The client must send a [HeartBeat](@ref doc-clientProtocol-command-control-heartbeat) packet every second to the proxy. If The heartBeats are missing, the connection is considered to be lost.
