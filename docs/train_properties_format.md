The format of train_properties {#train_properties_format}
========================

The train properties descibes the properties of different train models.
The following json fields exists for format version 0.6.0 of libtrainsim:

**required:**

* name (string) (since 0.4.0)
* mass (float) (since 0.4.0)
* maxPower (float) (since 0.6.0)
* surfaceArea (float) (since 0.9.0)
* numberWagons (int) (since 0.9.0)
* wagonLength (float) (since 0.9.0)
* driverLength (float) (since 0.9.0)

**optional:**

* formatVersion (string) (since 0.4.0)
* powerUnit (string) (since 0.6.0)
* trainType (string) (since 0.9.0)

**deprecated:**

* velocityUnit (string) (removed in 0.6.0)
* maxVelocity (float) (removed in 0.6.0)
* maxAcceleration (float) (removed in 0.6.0)
* airDrag (float) (removed in 0.9.0)
* trackDrag (float) (removed in 0.9.0)

### Detailed descriptions

#### name

The name of the train that can be used to be displayed in an UI.

#### mass

The mass of the train in kg.

#### maxPower

The highest power this train model can output in W.

#### surfaceArea

The effective surface Area of the front of the train in m^2.
This is needed to calculate the air Drag on the train.

#### numberWagons

The number of wagons this train has which are dragged along.

#### wagonLength

The length in m each wagon has.

#### driverLength

The length of the actual driver part of the train.


### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

### powerUnit

If this field is used the unit of maxPower can be manually set to something other than W.

**The possible values are:**

* W (default) (since 0.6.0)
* kW (since 0.6.0)

All other values will be ignored and it will be assumed that W is used.

### trainType

This field specifies the type of the train.

**The possible values are:**

* passenger (default) (since 0.9.0)
* cargo (since 0.9.0)

All other values will be ignored and it will be assumed that passenger is used.
