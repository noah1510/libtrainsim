# libtrainsim  {#mainpage}

This repository contains several components that are useful if you want to simulate a driving train.

To build this repository you need to install meson and opencv, all other dependencies should be downloaded by meson and compiled if they are needed but not installed (compiling opencv takes forever so there is no wrap file for it).

The documentation is built using doxygen an should be published to pages on every commit.

## Components

There is a build option to disable the build of the Video and Control component.

### Core

The core is what always needs to be built in oreder for the other components to work.
libtrainsim::core contains classes needed to load information about tracks and trains and perform some simple pysics calculations.

### Video

The Video component handles video files and can be used for simple playback.
It is based on opencv and requires opencv 2.4.0 or newer to work.

### Control

***This is not really implemented yet***
The control component manages input from various sources and gives an easy interface to handle them.
