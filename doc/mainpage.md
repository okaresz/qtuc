QtuC		{#mainpage}
==========

A fast prototyping framework for embedded systems, in a client (GUI) - server
(proxy) configuration.

# Motivation #

Are you fed up with the common practice that at the beginning of every project,
you spend a ridiculously long time implementing the same trivial tasks (serial
communication, communication protocolls, PC-side control program, etc..) again
and again?
Because I was, and so QtuC was created. QtuC takes care of these "basic"
features so that you can concentrate on the *real* developement.

# Components #

The framework consists of the following separate applications:

  * **qcDevice**: The code running on the embedded device (denoted as *device*
throughout the documentation). This sofware is not part of the provided
framework, only some bare minimum examples are provided, the implementation is
up to the device developer.
  * **qcProxy**: This is a *command-line* application, running usually on an x86
platform, and uses the Qt C++ framework.
qcProxy is in a tight communication loop with the device through a serial line
(though different protocols can be implemented if necessary),
and keeps an up-to date state of the device, serving as a high level API to the
device.
  *  **qcGUI**: The GUI can connect to the proxy through a TCP socket on a
custom port (can be set in the qcGuiSettings.xml).
If the connection is successful, the GUI will show the state of the device and
provides basic control.

# Basic concept and structure #

The state of the device is managed through variables, called \Device State
Variables. For example a variable can be a state of a LED in the embedded
system,
or an ADC channel reading, a timer value, etc... These variables are updated to
the proxy, to be easily accessible through qcGUI or any other connected client.
The device and the proxy communicates with a fixed command syntax, called
deviceCommand. See Device Command documentation page. Between the proxy and the
clients,
a custom XML-based packet communication exists. See [clientCommand](@ref
doc-clientProtocol) page for details.

The client and  device side is aupdated and handled asynchronously.

## DeviceAPI ##

You can manage and change information between the device and the proxy in two
ways: variables (grouped by the hardware interfaces, and functions).

#### Variables ####

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
  * double

##### Value conversion #####

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

#### Hardware Interfaces ####
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

#### Functions ####

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

A function can accept a custom number of arguments. See [deviceCommand](@ref
doc-deviceCommand) page for details.


## Settings ##

### deviceAPI.xml ##

You can define all the device-specific custom informations in this file. The
hardware interfaces, the state variables, and the functions, with several
optional additional paramater, for example 
how to display a state variable on the GUI, or how often should autoUpdate
update this variable, etc... See [deviceAPI doc page](@ref doc-deviceAPIxml) for
details.

### qcProxySettings.xml ##

Currently, this file exists beside the qcProxy executable, and generated
automatically with default values if not found.
See related page for details.

### qcGuiSettings.xml ##

Currently, this file exists beside the qcGUI executable, and generated
automatically with default values if not found.
See related page for details.
