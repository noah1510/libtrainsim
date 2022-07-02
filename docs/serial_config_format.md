The format of the serial configuration {serial_config_format}
========================

The serial interface needs a few configuration options.
All the fields are defined in here.

**Required:**

* comport (string) (since 0.7.0)
* baudrate (integer) (since 0.7.0)
* channels (array) (since 0.7.0)

**Optional:**

* formatVersion (string) (since 0.7.0)

**Channels:**

Each channel is a json object with the following attributes:

* name (string) (since 0.7.0)
* channel (integer) (since 0.7.0)
* type (string) (since 0.7.0)
* direction (string) (since 0.7.0)

### Detailed descriptions

#### comport

The name of the port the serial control is connected to.

#### baudrate

The baudrate used for the rs232 communication with the controls.
Usually should be 9600.

### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

### channels

#### name

The name to identify the channel in the software.
Input channels can be read from the serialcontrol by name and Output channels can be written to by name.
The required channels for core control functions are:

* analog_drive (type: analog) (since 0.7.0)
* analog_brake (type: analog) (since 0.7.0)
* emergency_brake (type: digital) (since 0.7.0)

#### channel

This specifies the device channel for this object.
Each device channel can be used for both a digital and analog input/output.
However outside of this they should be unique.

#### type

This specifies if this channel is either digital or analog.

#### direction

This specifies the data direction.
Input for reading data from the hardware controls, output to write data to the hardware controls.
