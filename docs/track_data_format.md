The format of track_data {#track_data_format}
========================

The track_data is a top level json array containing several data points.
The format version should be specified in the track, additional fields will be ignored.
Each data_point has the following fields:

**Required:**

* frame (int) (since 0.4.0)
* location (float) (since 0.4.0)

**Optional:**

* slope (float) (not implemented yet)
* acceleration (object) (not implemented yet)

### Detailed descriptions

#### frame

The number of frame that this point descibes.

#### location

The location of this point along the track in m.

#### slope

***This field is not implemented yet***

This field stores the rotation of the train to allow a calculation of gravity.

#### acceleration

***This field is not implemented yet***

This field stores the acceleration of the train mesured by an gyroscope.
The object has the following fields:

* x (float) The acceleration in x direction
* y (float) The acceleration in y direction
* z (float) The acceleration in z direction
* orientation (string) which axis shows the direction along the track
