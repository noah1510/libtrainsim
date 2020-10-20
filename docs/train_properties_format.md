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

* trackDrag (float) (since 0.4.0)
* airDrag (float) (since 0.4.0)

### Detailed descriptions

#### name

The name of the train that can be used to be displayed in an UI.

#### mass

The mass of the train in kg.

#### maxVelocity

The highest Velocity this train model can drive.

#### maxAcceleration

The highest Acceleration this train model can drive.

#### trackDrag

The rolling resistance coefficent between the train and the tracks.
The default is 0.002.

#### airDrag

The Front Area of the train multiplied by the Cw value.
The default value is 0.
