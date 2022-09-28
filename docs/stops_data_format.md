The format of stops {#stops_data_format}
========================

Each stops object has the following fields.

**Required:**

* name (string) (since 0.9.0)
* location (float) (since 0.9.0)
* type (string) (since 0.9.0)

### Detailed descriptions

#### name

This is a name for the stop which can be used to be displayed in the menus to allow selecting a specific stop.

### location

The location along the track where this stop is.
This value has to match one of the track data points.

### type

This string desctibes the type of this stop, this can be used to specify if the stop is a signal, station or something else.

**Possible values:**

* station (since 0.9.0)
