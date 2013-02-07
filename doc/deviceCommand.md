Device Command		{#doc-deviceCommand}
====================

The device command is a cleartext message between the device and the proxy, always terminated by a single newline character '`\n`'.

**General command syntax:**

`<type> <hwI> <var> [<arg> [<arg>] [...] ]\n`

The command consists of several chunks, separated by space. Double quotes can be used to escape chunks containing spaces.
Commands are and should be parsed case-sensitively throughout the framework.

# Type #	{#doc-deviceCommand-type}
The command type can be:

  * `set`
  * `get`
  * `call`

## set ##	{#doc-deviceCommand-type-set}

The set command always represents a write action. If the device gets a set command from the proxy, the embedded software should update the variable described in the command.
If the device sends a set command to the proxy, the variable will be updated on the proxy.

**Positive acknowledgement**<br>
If device uses positive ACK, after receiving a set command and successfully updating the variable, a reply with a similar set command, containing the new value must be sent back to the proxy. If an error occurs and the variable can't be updated, a reply is not needed.

Without positive ACK, the proxy sends out the set command, but no acknowledgment will be received from the device, whether the set command was successful is unknown.
Positive ACK is safer but a bit slower.

You can notify the proxy if the device uses positive ACK in the deviceAPI.xml, [deviceInfo](@ref doc-deviceAPIxml-deviceInfo) node, or in the [greeting message](@ref doc-deviceCommand-special-greeting).


## get ##	{#doc-deviceCommand-type-get}

Both the device and the proxy can read stateVariables from each other with the get commands.

## call ##	{#doc-deviceCommand-type-call}

You can call functions with the call command, with optional arguments.

# Hardware Interface (hwI) #	{#doc-deviceCommand-hwi}

The second chunk of the command always denotes the harware interface of the actual variable or function.

# var #		{#doc-deviceCommand-var}

In the case of a set/get command, the third chunk is the name of the variable we want to read/write. In the case of the call command, the name of the function we want to call.

# arg #		{#doc-deviceCommand-arg}

In the case of a get command, the fourth chunk is meaningless, since we only need the variable name to reply the value.

If the command is a `set`, then this is mandatory, meaning the new value for the variable.

With the call command, the argument is optional. If you want, you can pass several arguments, separated by spaces. If the argument itself contains space, you can escape the whole argument (for example a string) with double quotes.

In the case of a set command, if you want to set a new value for an integer variable, you can use the common integer prefixes: `0x` for hex, `0` for oct, non-prefixed numbers are treated as  decimal.

# Examples #	{#doc-deviceCommand-examples}

## set examples ##		{#doc-deviceCommand-examples-set}

`set drive speedRight 0x66\n` <br>
`set drive speedLeft 92\n` <br>
`set led debug on\n` <br>

## get examples ##		{#doc-deviceCommand-examples-get}

`get drive encoderLeft\n` <br>
`get ADC ch3\n`

## call examples ##		{#doc-deviceCommand-examples-call}

`call system reset\n` <br>
`call drive eStop\n` <br>
`call lcd write "Hello LCD"\n`

**Note the compulsory terminating newline at the end of every command.**


# Special commands #	{#doc-deviceCommand-special}

## Message ##		{#doc-deviceCommand-special-message}

With a device message the device can notify the proxy that something has happened. This could be an information about a state change, a simple debug message or an error report, etc...

There are four types of messages:

  * **info**: An information which could be helpful for the proxy and the user.
  * **debug**: A debug message which can be displayed if necessary (depending on the verbosity level)
  * **warning**: A warning message that something went wrong, but it's not fatal
  * **error**: Error message, meaning that a critical and possibly fatal error has happened.
  
The device can send these messages with the following syntax:

~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
call :proxy message debug ADC has started
call @e2fa5 :proxy message warning "Temperature is too high" 56
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   
The messages are usually forwarded to the clients with the [Message](@ref doc-clientProtocol-command-control-message) command.


## Greeting message ##		{#doc-deviceCommand-special-greeting}

The greeting message should be the first command sent to the proxy. It can contain several important parameters about the device and an optional greeting message.
The greeting also allows the proxy to detect device startup and reset, which can be a crutial information to the proxy and the users.

The syntax is the following:

    call @32a :proxy greeting "name:qcDeviceDemo" "desc:Demo for qcDevice" "platform:stm32f4xx" "positiveAck:0" "msg:Hey!"
    
The parameter key and value is separated with a colon. The key is parsed until the *first* occurence of the ":" character.

As you may notice, these are more-or-less the same parameters which can be given in the deviceAPI. The parameters received in a greeting message always overwrite the deviceAPI.
If an existing parameter is overwritten, qcProxy and the clients may warn about the possible mismatch between the device and the deviceAPI file.

If the device uses timekeeping, it is highly recommended to include a timestamp at least in the greeting message.