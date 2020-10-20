The format of track {#track_format}
========================

Each track is definied by a number of properties.
The json file is an object with the following fields:

**Required:**

* name (string) (since 0.4.0)
* data (string or [array](@ref track_data_format)) (since 0.4.0)
* train (string or [object](@ref train_properties_format)) (since 0.4.0)

**Optional:**

* startingPoint (float) (since 0.4.0)
* endPoint (float) (since 0.4.0)
+ stops (string or object) (not implemented yet)

### Detailed descriptions

#### name

This is a name for the track which can be used to be displayed in the menus to allow selecting a specific track.

#### data

This is either the filepath of a track data file or it is an array which contains the data in the correct [format](@ref track_data_format).
The filepath is relative to the track file.

#### train

This is either the filepath of a train properties file or it is an object which contains the data in the correct [format](@ref train_properties_format).
The filepath is relative to the track file.

#### startingPoint

This specifies where the course should start.
If this value higher than endPoint, the config is invalid.
If this value is negative, this should be ignored.

The default is 0.

#### endPoint

This specifies where the course should end.
If this value is negative or lower than staringPoint, the config is invalid.
If this value is higher than the last position on the track, this should be ignored.

The default is the last position in the track data.

#### stops

***This field is not implemented yet***

This is either the filepath of a stops data file or it is an array which contains the data in the correct [format](@ref stops_data_format).
The filepath is relative to the track file. 
