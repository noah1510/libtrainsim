The format of stops {#stops_data_format}
========================

***This is not implemented yet and will not be implemented before version 1.0.0***
The stops are a root array of stop objects with the following properties.

**Required:**

* name (string)
* location (float)

**Optional:**

* type (string)

### Detailed descriptions

#### name

This is a name for the stop which can be used to be displayed in the menus to allow selecting a specific stop.

### location

The location along the track where this stop is.
This value has to match one of the track data points.

### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

### type

***At the moment the possible values are not specified***
This string desctibes the type of this stop, this can be used to specify if the stop is a signal, station or something else.

**Possible values:**

* signal
* station
