The format of train_properties {#train_properties_format}
========================

The train properties descibes the properties of different train models.
The following json fields exists for format version 0.6.0 of libtrainsim:

**required:**

* name (string) (since 0.4.0)
* mass (float) (since 0.4.0)
* maxPower (float) (since 0.6.0)

**optional:**

* formatVersion (string) (since 0.4.0)
* trackDrag (float) (since 0.4.0)
* airDrag (float) (since 0.4.0)
* powerUnit (string) (since 0.6.0)

**deprecated:**

* velocityUnit (string) (removed in 0.6.0)
* maxVelocity (float) (removed in 0.6.0)
* maxAcceleration (float) (removed in 0.6.0)

### Detailed descriptions

#### name

The name of the train that can be used to be displayed in an UI.

#### mass

The mass of the train in kg.

#### maxPower

The highest power this train model can output in W.

### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

#### trackDrag

The rolling resistance coefficent between the train and the tracks.
The default is 0.02.
***At the moment this is used for the standing drag coefficient this will change in one of the next format versions!***

#### airDrag

The Front Area of the train multiplied by the Cw value.
The default value is 0.

### powerUnit

If this field is used the unit of maxPower can be manually set to something other than W.

**The possible values are:**

* W (since 0.6.0)
* kW (since 0.6.0)

All other values will be ignored and it will be assumed that W is used.
