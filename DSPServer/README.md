# DSPServer
This folder contains a first complete DSP-server project: **ServerSynth1** is a new polyphonic synthesizer which is VERY much a work-in-progress. The project references all the files under the **AudioKit** folder tree (one level up); the source files in this folder are representative of what any DSP server program needs.

Go to the **WebGUI** folder (one level up) to see how these are used.

## ServerSynth1

This folder was originally called *DSPServer*, but was renamed as there will eventually be many sibling folders.

Right now, there is only a MS Visual Studio 2017 project here, but the plan is to add projects for other compilers/IDEs, most particularly
- Xcode 9.x (Mac build)
- gcc/Makefile (Linux, *BSD, etc.)
- JetBrains [CLion](https://www.jetbrains.com/clion/) (Windows, Mac, and Linux)

## ServerSARAH

Presently just a clone of **ServerSynth1**. The goal is to create a version of my [SARAH](https://github.com/getdunne/SARAH) frequency-domain synthsizer for use on a server.
