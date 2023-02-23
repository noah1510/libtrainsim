The format of the simulator configuration {#simulator_configuration_format}
========================

The simulator configuration contains all of the options needed to have a running simulator.
This includes an array with all of the tracks, an array with additional trains to choose from and the configuration for the serial input.
It was added as part of format version 0.7.0 (libtrainsim version 0.10.0).
The json file is an object with the following fields:

**Required:**

* serialConfig (string) (since 0.7.0)
* tracks (array of string or [object](@ref track_format)) (since 0.7.0)
* trains (array of string or [object](@ref train_properties_format)) (since 0.7.0)

**Optional:**

* formatVersion (string) (since 0.7.0)
* defaultTrack (int) (since 0.7.0)
* settingFileReadOnly (bool) (since 0.8.0)
* shaderLocation (string) (since 0.8.0, optional since 0.9.0)
* textureLocation (string) (since 0.9.0)
* extrasLocation (string) (since 0.9.0)

### Detailed descriptions

All relative filepaths are relative to the simulator configuration file.

#### serialConfig

This is the relative file path to the [serial configuration file](@ref serial_config_format).

#### tracks

This is the array containing all of the tracks available to the simulator.
Each element in the array is either a [Track object](@ref track_format) or a relative filepath to a Track file.
At least one Track must be specified.

#### trains

This is an array containinf all of the additional trains selectable in the simulator.
Each element in the array is either a [Train object](@ref train_properties_format) or a relative filepath to a Train file.
The field is required but no extra train needs to be specified.

#### formatVersion

The version of the format that is used.
The format should be x.y.z and this tells the the library how to interpret the fields.
If the library only supports an older format it may warn the user or even declare the file as invalid.

#### defaultTrack

This specifies the index of the track that is selected by default.
If the field does not exist track 0 is selected by default.

#### settingFileReadOnly

This field specifies if the library is not allowed to change the settings file.
If this field does not exist it is set to false (modifications are allow) by default.

#### shaderLocation

The folder where all of the shader files are stored in.
For the basic video module at least the displacement shader is required.
Modules in the extra namespace should have their shader in a separate Folder.
By default this is "shaders".

#### textureLocation

The folder where all of the texutres are stored in.
The video module requires there to be the darken and displacement-0 texture to work.
Modules in the extras namespace should not have their textures here but instead in their separate folder.
By default this is "textures".

#### extrasLocation

The folder were all of the assets for the extras modules are stored.
By default this is "extras".
This folder is supposed to have a folder in it for every module.
How the files inside of each of those are structured is dependend on each module.
