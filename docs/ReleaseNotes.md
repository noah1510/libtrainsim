# release notes

This document contains all release notes.

-----------------------------------------------------------------------------

# Version 0.12.0 - VFX basics

This release has a working implementaion for snow effects that can be overlayed on the video.

## multi threaded video decode

The video decoder now uses multiple CPU core if there are more than 4 CPU threads available.
This causes huge performance gains and the decode to be a lot more stable.
The whole decoder is now a decode worker running on a different thread from which new frames can be requested.
Once the decode is finished the data is in active decode buffer.
Every time a decoded frame is received, the decode buffer gets swapped.
For the smoothest decode there are 3 decode buffers, which should prevent most problems that could happen.
If the requested frame is more frames in the future than a threshold, that frame is actually seeked which was not possible before.

## moving window and render code to SimpleGFX/SimpleGL

All new window parts are now in a separate library.
This cleans up the code a lot, since that code is not really specific to libtrainsim.
In addition to that the window parts can be better tested, reused and updated.
SimpleGL v0.1.0 is simply the latest video parts from libtrainsim as a standalone library.

The new video parts adds tabPage and window classes.
These allow adding a tab to a settings window and the window class for subwindows of the main window.
Since a rework of that library to gtk4 (or rather gtkmm4) is WIP, the API for windows and tabs will change a lot in the near future.

The core shader code is now provided by the SimpleGL library and no longer needs to be loaded from the file system.
Because of that they can more easily share the same shader parts (all use the same vertex shader) and other shader can use then without needing to load all parts from the file system.

## initial VFX

The initial VFX code contains snow, the ability to darken the output and a wiper for snow.
The darken textures can be retreived from the imguiHandler.
The other VFX parts are in the extras module and have individual meson options to enable/disable them.
More effects are coming in the future and most of the work is prepared to write more.

While up to 15 images may be overlayed on the video, setting a displacement map for the video is not possible yet (planned for v0.12).

## More platform compatibility

This release works on Linux (x64 and arm64), Windows (x64 Msys) and OSX (intel (arm not tested)).
To get that amount of platform support, the opengl requirement has been set to 3.3 core.
In the future the GL version might be changed to GLES 2 or 3 to support more devices however more tests are needed to check if that is even viable.

If you want to run the simulator on a raspberry pi 4, you have to override the mesa OpenGL version.
It works both with V3D and zink as driver (both need the override).
However even though it works on the raspberry pi, the frametimes are not good.

## Format version 0.9.0

Format 0.9.0 has all parts for a more accurate physics simulation, more UI and more rendering related features.

### simulator_configuration_format:

**optional:**

* shaderLocation (string) (since 0.8.0, optional since 0.9.0) The location where default shader files are stored
* textureLocation (string) (since 0.9.0) The location where texture files are stored
* extrasLocation (string) (since 0.9.0) The location where all the extra module specify files are stored

### track_definition:

**optional:**

* defaultTrackFrictionMultiplier (float) (since 0.9.0) The friction multiplier that is used in case a data point has none
* stops (array of stops objects) (since 0.9.0) An array containing all stops
* excludeTrackBounds (bool) (since 0.9.0) An option to add the begin and end as an extra stop
* undergroundData (array of undergroundData objects) (since 0.9.0) The information from where to where the train is underground


### stops_format:

**required:**

* name (string) (since 0.9.0) the display name for that stop
* location (float) (since 0.9.0) the location along the track for that stop
* type (string) (since 0.9.0) the type of this stop (atm only station)

### track_data_format:

**optional:**

* slope (float) (since 0.9.0) The slop of the track in degrees
* radius (float or string) (since 0.9.0) The bend radius of the track (infinity by default)
* frictionMultiplier (float) (since 0.9.0) The friction multiplier for that point on the track

### train_properties_format:

**required:**

* surfaceArea (float) (since 0.9.0) The effective surface area of the front of the train
* numberWagons (int) (since 0.9.0) The number of unpowerd wagon attached to this train
* wagonLength (float) (since 0.9.0) The length of the unpowered wagons
* driverLength (float) (since 0.9.0) The length of the driving part of the train

**optional:**

* trainType (string) (since 0.9.0) The type of this train (cargo/passenger), passenger by default

**deprecated:**

* airDrag (float) (removed in 0.9.0)
* trackDrag (float) (removed in 0.9.0)

-----------------------------------------------------------------------------

# Version 0.11.0 - Video rework

This release is a complete overhaul of the video module.

## new video decoder

The video decoding class has be fully rewritten and can now actually skip ahead in the movie file instead of only advancing a frame.
This increases the performance by a huge amount especially if the video has to be advanced more than a few frames.

## switch to imgui

The biggest change is the move from pure sdl2 window management to imgui with opengl4.6 and sdl2.
This allows for more than window to be part of the application.

### status display

To demonstrate this all of the status variables like speed, framerate, position, etc. are no longer displayed using the terminal but instead using a window that is part of the extras module.
For more other custom windows the statusDisplay module is a good starting point since it is a minimal implementation and should be easy to understand.

### new video class and texture class

The new video class is now longer a singleton.
Because of this a new instance can be created once you are done with a track.
In theory this is the least, that is needed to have a fully functional UI with a track selection and a settings window (those still have to be implemented).
Another feature of the new video manager is that is allows overlaying a texture on top of the video.
This allows other modules to create overlays like weather or HUDs which can be attached to the main video window.
Please note that this requires some knowledge of opengl since you have to render you overlay into a texture using a framebuffer.
The texture class tries to make it as easy as possible but you still have to create all of the object buffers and use your own shader.

### shader class

There is a shader class which allows for easy management of opengl glsl shaders.
This class handles loading the shader files from storage, compiling and linking them into a program.
There are also a lot of functions to set uniform variables and a function to easily use the shader for the next render call.
It still requires some basic knowledge of opengl to be used but makes the shaders actually manageable.

## format 0.8.0

The new json format version has two new fields in the simulator_configuration.
The first one is the folder that contains all of the shader files (it is required so make sure to update your settings).
The second one is a flag to notify that the setting files should be read only.
It is not used yet but is really useful/necessary once settings windows are there.
On some installation methods you are not allowed to change these files so this is a preparation for that.


-----------------------------------------------------------------------------

# Version 0.10.0 - input and error handling update

This release brings quite the large change to libtrainsim.
Hardware input was added, there is now a simulator configuration and most of the error handling was rewritten.

## input updates

There is now working hardware input :tada:.
If you have hardware connected you can specify the used serial port in the serial configuration file.
The default port is `/dev/ttyACM0` so if you use windows you have to change it.
The input handler class uses the serial input by default when it is available.

## simulator configuration

Now there is a simulator configuration file.
It specifies all of the tracks that can be used by the simulator, the location of the serial config file and additional trains that may be used on all tracks.
If wanted the index of the default track can also be provided, so that the used track can be changed from the config file.
**The reference implementation now needs a simulator configuration instead of a Track configuration to start.**

## Helper class

There now is a helper class to provide useful functions throughout the library.
At the moment the following functions are provided:

* printException -> prints a (nested) exception to make diagnosting runtime problems easier
* getJsonField -> gets a field from a json object and throws exceptions if it does not exist or has the wrong type
* getOptionalJsonField -> like getJsonField but no error when it does not exist

More functions will be added in the future.

## Error handling

All of the core classes now throw exceptions when something goes wrong in the constructor.
The exceptions can be nested to get more details on what went wrong during the runtime without the need for a debugger.
To output these exceptions in a nice way the helper class may be used.
