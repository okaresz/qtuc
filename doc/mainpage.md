Overview		{#mainpage}
==========

A fast prototyping framework for embedded systems, in a server - client configuration.


# Motivation #		{#mainpage-motivation}

Are you familiar with the common practice that at the beginning of every project,
you spend a ridiculously long time implementing the same trivial tasks (serial
communication, communication protocolls, PC-side control program, etc..) again
and again?

QtuC takes care of these "basic" features so you can concentrate on the *real* developement.


# Components #			{#mainpage-components}

The framework consists of the following separate applications:

  * **QcDevice**: The code running on the embedded device (denoted as *device*
throughout the documentation). This sofware is not essential part of the provided
framework, only some bare minimum examples are provided, the implementation is
up to the device developer.
  * **QcProxy**: This is a *command-line* application, running usually on an x86 / x86_64
platform, and uses the Qt C++ framework.
QcProxy is in a tight communication loop with the device through a serial line
(though different protocols can be implemented if necessary),
and keeps an up-to date state of the device, serving as a high level API to the
device.
  *  **QcGUI**: The GUI can connect to the proxy through a TCP socket on a
custom port.
If the connection is successful, the GUI will show the state of the device and
provides basic control.


# Basic concept and structure #			{#mainpage-concept}

The state of the device is managed through variables, called Device State
Variables. For example a variable can be a state of a LED in the embedded
system,
or an ADC channel reading, a timer value, etc... These variables are updated to
the proxy, to be easily accessible through qcGUI or any other connected client.
The device and the proxy communicates with a fixed command syntax, called
deviceCommand. See [Device Commands](@ref doc-deviceCommand) documentation page. Between the proxy and the
clients,
a custom XML based packet communication exists. See [clientCommand](@ref doc-clientProtocol) page for details.

The client and  device side is updated and handled asynchronously.


## DeviceAPI ##			{#mainpage-concept-deviceAPI}

You can manage and change information between the device and the proxy in two
ways: variables (grouped by the hardware interfaces, and functions).


### Variables ###			{#mainpage-concept-deviceAPI-variables}

In most cases, the variable-based workflow is recommended.
Whenever you want to monitor/control a particular state of the device (state of
a GPIO input, an ADC value...), you must define a variable (aka. stateVariable)
for it.

The required components are usually a name, a type and the hardware interface
the variable is belonging to.

The state variables in the framework are represented by the DeviceStateVariable
class.

The stateVariables are handled with a fixed type. In the deviceCommand, all
variables are represented with a string value, however these values are parsed
and cast according to the specified variable type.

**Available types** <br>
  * string
  * boolean
  * integer
  * unsigned integer
  * double

  
#### Value conversion ####			{#mainpage-concept-deviceAPI-variables-conversion}

When working with embedded devices, the values in the device are often raw,
hardware-specific numbers, strings... The values need conversion before a
high-level generic application or a human can understand and use them.
For this, the framework provides a value conversion feature.

All variable has two '*sides*'. A device-side raw value and a user-side (converted) value. The device side is what the proxy gets from the device in a
deviceCommand. Usually a raw, device-specific number.
This is cast to the right type in the proxy, and a converted, user-side 'value' is
generated according to the user-defined conversion script. This value then can
be queried form the proxy to display on a GUI, or used otherwise.
An example would be an incremental encoder on a motor axis. From the device
point-of-view,m this is an unsigned integer. However, the user probably wants to
read the information in degrees, which is a floating point variable, or double.
As you can see, not just a value conversion is needed, but also the type is
different. These different types for the device and the gui side can be defined
in the [deviceAPI file](@ref doc-deviceAPIxml), along with the conversion
scripts for both ways.


### Hardware Interfaces ###			{#mainpage-concept-deviceAPI-hwInterfaces}

For better usability and easy overview, the variables are grouped by *hardware
interfaces* (referred sometimes as `hwI`). A hardware interface represents a
closely-coupled set of features (variables and functions) on the device.
For example `GPIO` can be a hardware interface, with several general IO related
state variables. Or `ADC` can be another, and so on... It's a better idea
though, if you name the hardware interfaces
after the function of the interface in the actual application environment, and
not the uC module handling it. For example a good name would be `drive`,
consisting the motor control variables such 
as the PWM duty ratio, the encoder reading TIMER value, the current measuring
ADC, etc... And of course some related functions, such as emergencyStop().


### Functions ###			{#mainpage-concept-deviceAPI-variables}

Whenever you can, use a stateVariable to communicate with the device. If a
feature / state cannot be solved with a variable, or is inconvenient or not
logical, you can use a function.
For example it would be unnecessary to define a variable just for a software
reset request. A function can also be handy if a fast and short respond is
needed from the device, so defining an emergencyStop() function
is a better solution than stopping maybe several motors through their speed
variables, one after another...

The downside of a function is that you must implement the call command handling
in the proxy code yourself (if the device sends a call command to the proxy,
that is. From the proxy to the device, any number of functions can be defined in
the device API file, they will be accessible on the GUI without any modification
to the proxy or gui code.)

A function can accept a custom number of arguments. See [deviceCommand](@ref doc-deviceCommand) page for details.


## Timekeeping ##			{#mainpage-concept-timekeeping}

The communication between the device and the proxy, and even more between the proxy and the clients takes a considerable amount of time. The clients are connected through a TCP socket, which - when there is a large network distance between the endpoints - brings a random delay to the system.
Some  applications however require precise timing, such as logging or graph plotting. To tackle this problem, QtuC supports a timekeeping method.

In timing-critical applications, the device should implement a timekeeping mechanism, such as a system timer. This way when a variable is requested from the device, it can generate a timestamp, marking the creation time of the value.
This timestamp can be sent with the `set` command reply in the following form:

    set @35d2 hwiname varName value

the timestamp is the second word, a hex number after the command name (`set`), prefixed with a `@` sign, marking the time since device startup.
This timestamp is treated as the passed time since device startup. The granularity may vary from device to device, and can be set with defining the device time ticks per millisecond.
Since the timekeeping in the proxy and in general on a PC platform is millisecond based (better granularity is rarely available depends on the OS), the device timestamp must be converted to millisecond granularity.
The constant for this conversion is `deviceTimeTicksPerMs`. This is a double number, meaning the device timer ticks per millisecond. This can be of course less than one, if the timer is slower than 1000Hz. Zero value is invalid.
DeviceTimeTicksPerMs can be set in two ways:

  * In the qcProxy configuration file, device section, key *timeTicksPerMs*
  * Can be sent to the proxy as a parameter in the device greeting message with the name of `timeTicksPerMs`

For slower devices, or when time is not critical, a timer frequency of 1kHz is acceptable. For higher-end, faster devices however it's recommended to keep a 1MHz timer (microsecond granularity).

QcProxy always registers when a stateVariable was last updated. If the device doesn't send timestamps, this update time will be the arrival of the command. If id does however, the following method is used:

Since qcProxy stores the variable last update time with a UNIX timestamp, it needs the device startup time to synchronize it with the device timestamp.
QcProxy will take the timestamp of the first valid device command (ideally a device greeting), convert it to millisecond granularity and substract it from the current UNIX timestamp, resulting in the device startup time. This serves as a reference point for the following relative device timestamps.
If the first device command doesn't contain a timestamp, the time of arrival will be used to calculate the startup time.

The calculated variable update time is included in the client commands as a UNIX timestamp. See [ClientCommandDevice](@ref doc-clientProtocol-command-device).

In passThrough mode, qcProxy timekeeping is disabled. The timestamps are however forwarded to the clients **unaltered**.

Since logging and plotting is the task of a client, the client must first acquire the stateVariable to log. This can be with explicit get command or with subscription.
Either way, normally it is not guaranteed that all update of the device variable will be cought by the client, but all registered updates will be stored with the correct time (not the time of arrival), thanks to the timestamps.
However, by using the proxy in passthrough mode, full synchronization without update loss is achievable (but as the timestamps will be forwarded by the proxy as-is, the client is responsible to convert them to absolute UNIX timestamps (or not)).


# Settings #			{#mainpage-settings}

## deviceAPI.xml ##			{#mainpage-settings-deviceAPIxml}

You can define all the device-specific custom informations in this file. The
hardware interfaces, the state variables, and the functions, with several
optional additional paramater, for example 
how to display a state variable on the GUI, or how often should autoUpdate
update this variable, etc... See [deviceAPI doc page](@ref doc-deviceAPIxml) for
details.

## qcProxy and qcGUI settings ##			{#mainpage-settings-progSettings}

Proxy and gui settings are managed with QSettings, for more information, see Qt's documentation on it.
However, the settings are explicitly written to file, even on windows, where the default QSettings backend is the registry.
The path of the current settings file is printed on the command line upon program startup.
Generated automatically with default values if not found.
See related page for details.


# Command line arguments #			{#mainpage-cmdLineArgs}

-v: be verbose

-vv: be very verbose

--passthrough: Enable passthrough mode. In this mode, proxy will relay all device commands to all clients, and send all client device commands to the device.
This way, deviceStateVariables in the proxy are not used, proxy will act only as a translator. Also, the commands related and relying on stateVariables will not work in passthrough mode, for example subscribe, autoUpdate, and so on...
