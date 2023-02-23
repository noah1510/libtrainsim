The format of track {#track_format}
========================

Each track is definied by a number of properties.
The json file is an object with the following fields:

**Required:**

* name (string) (since 0.4.0)
* data (string or [array](@ref track_data_format)) (since 0.4.0)
* train (string or [object](@ref train_properties_format)) (since 0.4.0)
* videoFile (string) (since 0.5.0)

**Optional:**

* formatVersion (string) (since 0.4.0)
* startingPoint (float) (since 0.4.0)
* endPoint (float) (since 0.4.0)
* defaultTrackFrictionMultiplier (float) (since 0.9.0)
* stops (array of stops objects) (since 0.9.0)
* excludeTrackBounds (bool) (since 0.9.0)
* undergroundData (array of undergroundData objects) (since 0.9.0)

### Detailed descriptions

#### name

This is a name for the track which can be used to be displayed in the menus to allow selecting a specific track.

#### data

This is either the filepath of a track data file or it is an array which contains the data in the correct [format](@ref track_data_format).
The filepath is relative to the track file.

#### train

This is either the filepath of a train properties file or it is an object which contains the data in the correct [format](@ref train_properties_format).
The filepath is relative to the track file.

#### videoFile

This is the filepath to the video relative to the track file.

#### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

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

#### defaultTrackFrictionMultiplier

This is the friction multiplier if the track data has no multiplier specified at a point.
Please note that this is only a fallback, if the track data has a value that will be used.

The default value is 1.0.

#### stops

This is an array which contains the data in the correct [format](@ref stops_data_format).

#### excludeTrackBounds

If this is true the Track will no longer return the begin and end of the track as a stop.
This is only considered if stops are defined.
If there are less that 2 stops this will always be treated as false.
By default this is true.

#### undergroundData

The underground data specifies from where to where the train is underground.
Each undergroundData object has two required fields:

* begin (float) (since 0.9.0) (The point in meters where the train starts being underground)
* end (float) (since 0.9.0) (The point in meters where the train stops being underground)

It also has one optional field:

* tunnelArea (float) (since 0.9.0) (The surface area of the tunnel)

If nothing is specified all of the track is assumed to be above the ground.

