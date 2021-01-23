# libtrainsim  {#mainpage}

This repository contains several components that are useful if you want to simulate a driving train.

To build this repository you need to install meson and a c++ compiler toolchain with support for C++17 (at least gcc-9.2.0 or clang-9).

The documentation is generated using doxygen an should be published to the wiki on every commit.
German translations are coming soon feel free to contribute to them.

**Please note that the windows build does not work at the moment. Look at issue #10 issue for more details.**

To see a basic reference implementation look at [this repository](https://git.thm.de/bahn-simulator/simulator).
It uses all parts of libtrainsim to create a functional train simulator with just a small codebase.

The main branch always contains the latest working code, while the stable branch contains the state of the last release.
If you want to have the most recent version without needing to worry about possible build problems use the stable branch.
Pull request should always be targeted at the main branch.

## Contributing

If you want to contribute please look at the issues first to find out what is being worked on at the moment.
You can fork the code and open a pull request with your changes.
Please note that all new functions need unit tests and at least English documentation to be accepted.
You can still open the pull request to ask for help and get more information on what is missing.

**All of the issues and pull requests need to be written in English or they will be automatically rejected!**
All new classes, functions and variables should be English words or abbreviations otherwise they will not be merged.

If you find a vulnerability please open a new issue and mark it as confidential.
This way the vulnerability is not public until a fix is found (which has priority over other work).
Please report which versions are affected by it and which version you tested, to allow easy backporting of the fix.
Before release 1.0.0 only the latest version will recieve updpates, so make sure you are on the latest version.

## Components

The library consits of several components.
Except for the Core component each individual component can be disabled and replaced by custom components to suit your needs.

### Core

The core is what always needs to be built in oreder for the other components to work.
libtrainsim::core contains classes needed to load information about tracks and trains.
Look [here](@ref json_formats) for details on the formatting of the json files.

### Video

The Video component handles video files and can be used for simple playback.
It has two possible backends: sdl2/ffmpeg and opencv.
By default all the backends that can be built will be built.
You have to manually install ffmpeg or opencv to get the build working.
The [setup-scripts](https://git.thm.de/bahn-simulator/setup-scripts) in the root folder automatically install all of the needed dependencies.

### Control

The control component manages input from various sources and gives an easy interface to handle them.
At the moment the only working input is the keyboard input privided by the current video backend.
Because of that this component is useless unless you have enables libtransim::video (at least at the moment).

### Physics

The physics component handles basic physics calculations.
It only needs libtrainsim::core to work.
At the moment things like drag are not included in the calculations and the speed is 'directly' controlled by providing the current accelleration.
Feel free to contribute to improve this module as much work is needed for a more realistic physics calculation.
