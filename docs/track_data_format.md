The format of track_data {#track_data_format}
========================

The track_data is a top level json array containing several data points.
The format version should be specified in the track, additional fields will be ignored.
Each data_point has the following fields:

**Required:**

* frame (unsigned int) (since 0.4.0)
* location (double) (since 0.4.0)

**Optional:**

* slope (float) (since 0.9.0)
* radius (float or string) (since 0.9.0)
* frictionMultiplier (float) (since 0.9.0)
* acceleration (object) (not implemented yet)

### Detailed descriptions

#### frame

The number of frame that this point descibes.

#### location

The location of this point along the track in m.

#### slope

This field stores the rotation of the train to allow a calculation of gravity.
The value has to be in degrees.
If no value is given this is set to 0.

#### radius

The radius of the Track at this point.
If the Track is straight at that point the radius should be set to the string INFINITY.
Otherwise it has to be set to a floating point value.
This value is the radius in km.
If no value is given it is set to INFINITY.

#### frictionMultiplier

The multiplier of the base friction value (0.3).
This may be used to specify the condition of the Track at this point.
If no value if given the defaultTrackFrictionMultiplier from the Track configuraion is used.

#### acceleration

This field stores the acceleration of the train mesured by an gyroscope.
The object has the following fields:

* x (float) The acceleration in x direction
* y (float) The acceleration in y direction
* z (float) The acceleration in z direction
* orientation (string) which axis shows the direction along the track

