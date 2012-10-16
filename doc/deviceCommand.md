Device Command		{#doc-deviceCommand}
====================

The device command is a cleartext message between the device and the proxy, always terminated by a single newline character '`\n`'.

**General command syntax:**

`<type> <hwI> <var> [<arg> [<arg>] [...] ]\n`

The command consists of several chunks, separated by space. Double quotes can be used to escape chunks containing spaces.
Commands are and should be parsed case-sensitively throughout the framework.

# Type #
The command type can be:

  * `set`
  * `get`
  * `call`

## set ##

The set command always represents a write action. If the device gets a set command from the proxy, the embedded software should update the variable described in the command.
After successfully updating the value, a reply with a similar set command, containing the new value must be sent back to the proxy. If an error occurs and the variable can't be updated, a reply is not needed.
If the device sends a set command to the proxy, the variable will be updated on the proxy.

## get ##

Both the device and the proxy can read stateVariables from each other with the get commands.

## call ##

You can call functions with the call command, with optional arguments.

# Hardware Interface (hwI) #

The second chunk of the command always denotes the harware interface of the actual variable or function.

# var #

In the case of a set/get command, the third chunk is the name of the variable we want to read/write. In the case of the call command, the name of the function we want to call.

# arg ##

In the case of a get command, the fourth chunk is meaningless, since we only need the variable name to reply the value.

If the command is a `set`, then this is mandatory, meaning the new value for the variable.

With the call command, the argument is optional. If you want, you can pass several arguments, separated by spaces. If the argument itself contains space, you can escape the whole argument (for example a string) with double quotes.

In the case of a set command, if you want to set a new value for an integer variable, you can use the common integer prefixes: `0x` for hex, `0` for oct, non-prefixed numbers are treated as  decimal.

# Examples #

## set examples ##

`set drive speedRight 0x66\n` <br>
`set drive speedLeft 92\n` <br>
`set led debug on\n` <br>

## get examples ##

`get drive encoderLeft\n` <br>
`get ADC ch3\n`

## call examples ##

`call system reset\n` <br>
`call drive eStop\n` <br>
`call lcd write "Hello LCD"\n`

**Note the compulsory terminating newline at the end of every command.**