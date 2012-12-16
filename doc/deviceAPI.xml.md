deviceAPI.xml		{#doc-deviceAPIxml}
===================

With this XML file you can define the "interface" for the device. Here you should list all the hardware interfaces, variables and functions you wish to use with the device.

The file **must be in the same directory as the qcProxy executable**. If this file is missing, the proxy will emit an error message and will exit immediately.

# Basic layout #		{#doc-deviceAPIxml-layout}

The basic structure of the file is the following:

~~~~~~~~~~~~~~~~~~~~~~~
<!DOCTYPE QtuCDeviceAPIDef>
<deviceAPI>

	<deviceInfo>
		...
	</deviceInfo>
	
	<hardwareInterfaceList>
		...
	</hardwareInterfaceList>

	<stateVariableList>
		...
	</stateVariableList>
	
	<functionList>
		...
	</functionList>

</deviceAPI>
~~~~~~~~~~~~~~~~~~~~~~~

# deviceInfo #		{#doc-deviceAPIxml-deviceInfo}

Some basic information about the device. This node is optional, but strongly recommended!

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<deviceInfo>
	<positiveAck>false</positiveAck>
	<name>deviceName</name>
	<desc>A short description of the device</desc>
	<platform>A short platform string, eg.: STM32F4</platform>
	<project>projName</project>
</deviceInfo>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**positiveAck**: Whether the device uses positive acknowledgment. Can be ''true'' or ''false''. If not defined, ''false'' will be used. See [set command](@ref doc-deviceCommand-set) for details.<br>
**name**: A short name of the device. (max 50 char, ASCII)<br>
**desc**: Description of the device.<br>
**platform**: A short platform description.<br>
**project**: A string to identify the project. What for? For example you have one device, but you use it in several projects, each needing a different API file.<br>

# hardwareInterfaceList #		{#doc-deviceAPIxml-hwList}

Here you can define the hardware interfaces used on the device. The hwI name will be chacked on every received command, and a warning will be generated if it's invalid, and the command will be dropped.

The node describing a hardware interface is as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<hardwareInterface>
	<name>hwI_name</name>
	<info>Hardware interface info.</info>	<!-- optional -->
</hardwareInterface>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**name**: Name of the hardware interface. The name can only use the english alphabet, numbers, and the underscore (regex: `[a-zA-Z0-9_]`).<br>
**info** (optional): A short description of the interface. Only plain text is allowed. This info can be accessed by any client at runtime,

Example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<hardwareInterfaceList>
	<hardwareInterface>
		<name>rgbled</name>
	</hardwareInterface>
	<hardwareInterface>
		<name>drive</name>
		<info>DC motor control and monitoring.</info>
	</hardwareInterface>
</hardwareInterfaceList>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


# stateVariableList #		{#doc-deviceAPIxml-stateVarList}

In this node you can define the various state variables you wish to use with the dervice.

The bare minimum information you must define is the name, the hardware interface, and the type of the variable:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<stateVariableList>
	<stateVariable>
		<hwInterface>hwI_name</hwInterface>
		<name>varName</name>
		<type>varType</type>
	</stateVariable>
</stateVariableList>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**hwInterface**: A valid hardware interface name. <br>
**name**: The name of the variable. Can only use the english alphabet, numbers, and the underscore (regex: `[a-zA-Z0-9_]`). <br>
**type**: The type of the variable. Possible values:
  * `string`
  * `int` or `integer`
  * `double`
  * `bool` or `boolean`

With only one value, the type of the variable will be tha same both on device and user sides.

There's a lot more options however to fine-tune the behavior of the variable. The full list is as follows:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<stateVariableList>
	<stateVariable>
		<hwInterface>hwI_name</hwInterface>
		<name>varName</name>
		<type>
			<device>varType_on_device</device>
			<user>varType_on_user_side</user>
		</type>
		<access mode="r|w|rw"/>
		<conversion>
			<toUser><![CDATA[Qt script]]></toUser>
			<toDevice><![CDATA[Qt script]]></toDevice>
		</conversion>
		<autoUpdate side="device">update_frequency_Hz</autoUpdate>
		<autoUpdate side="user">update_frequency_Hz</autoUpdate>
		<guiHint>
			...
		</guiHint>
	</stateVariable>
</stateVariableList>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**type**: If you want to define different types on the device and user side, you must define both of them with the `<device>` and `<user>` nodes. <br>

**access**: Controls different aspects of how to access the variable.
  * *mode*: I/O mode of the variable from the user's point of view. Default is `r`.
    * `r`: The variable can only be read from the device.
    * `w`: Write-only variable (no get commands can be sent to the device for this variable. However the proxy will keep track of the values set to this variable, so a client may read the value from the proxy (which may not be the same value as on the device).
    * `rw`: Read/write variable. Be careful when using this bi-directional mode! The proxy will handle both reads and writes to the device, but client and device-side implementation can be problematic.
  **Important!** If you would like to *write* a variable, you must explicitly specify the *mode* attribute. If you omit the whole *acces* node or just *mode* attribute, the default mode (read-only) will be set.

**conversion** (optional): Here can you define the conversion scripts between the sides. The script must be in a CDATA node. The script is parsed with [Qt's scripting API](http://qt-project.org/doc/qt-4.8/scripting.html).
Qt Script is based on the ECMAScript scripting language, as defined in standard ECMA-262. For more information, see the [Qt documentation](http://qt-project.org/doc/qt-4.8/scripting.html), or Qt's [ECMAScript reference](http://qt-project.org/doc/qt-4.8/ecmascript.html).<br>
In the scripts, the name of the current variable holds the current value in the source side. For example let's say we have a variable encVal1, meaning an incremental encoder on a motor axis. The value on the device is an integer, but we want to read it in degrees. If 100 pulses mean one revolution, then the toUser script would be: `encVal1 / 100 * 360`. See examples for the full node.<br>
*toUser*: The script to convert from the raw device side to the more general and meaningful value.<br>
*toDevice*: The script to convert from the user to the device side. <br>
You must define both sides (or none at all, omitting the whole conversion node).<br>
Please note that the value returned by the script will be cast to the type of the target side.

**autoUpdate** (optional): If this node is present, the variable will be updated periodically on the given side.
The value must be given as an integer, in Hz (updates per second).<br>
*side*: This attribute is compulsory, if omitted, the autoupdate definition will be dropped. Value is `device` or `user`. With this you can define different update frequencies on the two sides.
  * `device`: the variable will be pulled from the device periodically with the defined frequency. This means that the proxy will send a get command for the variable to the device periodically, to which the device must respond with the corresponding set command with the current value.<br>
  * `user`: The variable will be updated automatically on qcGUI with the given frequency.

**guiHint**: Under active development,

Example:
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<stateVariableList>
	<stateVariable>
		<hwInterface>led</hwInterface>
		<name>dbg</name>
		<type>bool</type>
	</stateVariable>
	<stateVariable>
		<hwInterface>drive</hwInterface>
		<name>encVal</name>
		<type>
			<device>int</device>
			<user>double</user>
		</type>
		<conversion>
			<toUser><![CDATA[encVal / 100 * 360]]></toUser>
			<toDevice><![CDATA[encVal / 360 * 100]]></toDevice>
		</conversion>
		<autoUpdate side="device">50</autoUpdate>
		<autoUpdate side="user">5</autoUpdate>
	</stateVariable>
</stateVariableList>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


# functionList #		{#doc-deviceAPIxml-funcList}

Here you can define the functions you want to call on the device. This node is otional.

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<function>
	<hwInterface>hwI_name</hwInterface>
	<name>funcName</name>
	<args> arg1 arg2 "argument 3 with spaces..."</args>	<!-- optional -->
</function>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

**hwInterface**: A valid hardware interface name. <br>
**name**: Name of the function. Can only use the english alphabet, numbers, and the underscore (regex: `[a-zA-Z0-9_]`). <br>
**args** (optional): Argument(s). If the argument data contains characters that violates the XML standard, or you're not sure, you can use CDATA (see example)

Example
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
<functionList>
	<function>
		<hwInterface>system</hwInterface>
		<name>reset</name>
	</function>
	<function>
		<hwInterface>LCD</hwInterface>
		<name>write</name>
		<args><![CDATA[I <3 Qt.]]></args>
	</function>
<functionList>
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

The device can also send call commands, which the proxy should execute. If you would like to handle a call command in the proxy, you must implement it to qcProxy.