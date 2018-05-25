# AudioKit-net
This repository features code to make audio DSP functions available across TCP/IP networks. There are at least four reasons to do this (click on the links for details):

1. To support and simplify [cross-platform audio DSP development](doc/cross-platform-dev.md).
2. To establish a [non-proprietary, platform-independent audio plug-in technology standard](doc/network-plugin-std.md).
3. To enable [network bridging](doc/network-bridging.md)--allowing audio plug-ins and client programs such as Digital Audio Workstations (DAWs) to run on different computers (or distinct run-time environments on a single computer).
4. To allow [standard servers to be used as hardware accelerators](doc/network-accel.md) for audio DSP plug-ins.

The basic networking protocol and techniques for network bridging were developed as part of the author's [NetVST Project](http://netvst.org/wiki). In the spring of 2018, it became obvious that the NetVST plug-ins could be used to bridge arbitrary DSP code to standard DAWs without additional platform-specific adapter code, facilitating both development and use of AudioKit Core DSP code on non-Apple platforms. This led to a decision to publish all relevant code as part of a new AudioKit repo (this one) on GitHub.

## Work in progress
This repo is still a work in progress. Only the primary documentation files have been published so far.
