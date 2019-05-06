# PSIORI ACT

A framework for pragmatic decision making in cognitive architectures.

# Install

## Build psi-act library

Create a build-directory for storing configuration and temporary files:

    > mkdir build

Change to directory:

    > cd build

Configure cmake and create make files (also builds the tools):

    > cmake -DCMAKE_BUILD_TYPE=Release ..

Build and install BrickTrack:

    > make
    > make install

This will "install" the library within the package at lib/. In order to install the library system wide you need to set the install path before running make install:

    > cmake -DCMAKE_INSTALL_PREFIX=/opt/local/ ../
    > make
    > sudo make install

Create documentation from sources:

    > make doc

The documentation can be found in
doc/html/index.html

You can create a fresh XCode project from scratch from the files and use
this to edit and build psi-act. Just run cmake's generator:

    > mkdir build_xcode
    > cd build_xcode
    > cmake -G Xcode ..

This will generate a new project psi-act.xcodeproj.

# Authors

- Sascha Lange <sascha@psiori.com>