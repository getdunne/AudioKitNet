# DSPServer
This folder contains a handful of DSP-server projects.

Go to the **WebGUI** folder (one level up) to see how these are used.

## ServerSynth1
**ServerSynth1** is a first complete DSP-server project: a new polyphonic synthesizer which is VERY much a work-in-progress. The project references all the files under the **AudioKit** folder tree (one level up); the source files in this folder are representative of what any DSP server program needs.

Right now, there is only a MS Visual Studio 2017 project here, but the plan is to add projects for other compilers/IDEs, most particularly
- Xcode 9.x (Mac build)
- gcc/Makefile (Linux, *BSD, etc.)
- JetBrains [CLion](https://www.jetbrains.com/clion/) (Windows, Mac, and Linux)

## ServerVST
**ServerVST** is a simple DSP-server that instantiates a single VST plugin and makes it accessible across the network. It is thus a poor-man's [NetVSTHost](http://netvst.org/wiki).

This program will probably be Windows-only for the foreseeable future, but I suppose a Mac version (to run VSTs compiled for macOS) is conceivable.

Lots of work-in-progress here to extend the original NetVST concept to support web-based remote GUIs.

## ServerSARAH
The goal is to create a version of my [SARAH](https://github.com/getdunne/SARAH) frequency-domain synthsizer for use on a server.

This code is barely started, and is not working yet.

