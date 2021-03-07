The format of train_properties {#train_properties_format}
========================

The train properties descibes the properties of different train models.
The following json fields exists for version 0.4.0 of libtrainsim:

**required:**

* name (string) (since 0.4.0)
* mass (float) (since 0.4.0)
* maxVelocity (float) (since 0.4.0)
* maxAcceleration (float) (since 0.4.0)


**optional:**

* formatVersion (string) (since 0.4.0)
* trackDrag (float) (since 0.4.0)
* airDrag (float) (since 0.4.0)
* velocityUnit (string) (since 0.4.0)

### Detailed descriptions

#### name

The name of the train that can be used to be displayed in an UI.

#### mass

The mass of the train in kg.

#### maxVelocity

The highest Velocity this train model can drive in m/s.

#### maxAcceleration

The highest Acceleration this train model can drive in m/s^2.

### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

#### trackDrag

The rolling resistance coefficent between the train and the tracks.
The default is 0.002.

#### airDrag

The Front Area of the train multiplied by the Cw value.
The default value is 0.

### velocityUnit

If this field is used the unit of maxVelocity can be manually set to something other than m/s.

**The possible values are:**

* ms (since 0.4.0) (m/s)
* kmh (since 0.4.0) (km/h)

All other values will be ignored and it will be assumed that m/s is used.
