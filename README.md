# AudioKitNet

This work was previously part of [AudioKit](https://github.com/AudioKit/AudioKit), and is related to my earlier [NetVST Project](http://netvst.org/wiki).
It is **not really in a working state anymore**; please be warned. I hope to get back to this soon, but I'm pretty busy with other things, e.g.
[Unify](https://www.pluginguru.com/products/unify-standard).

------

## (Nearly) Instant gratification
There are a few steps to go through before you can play with this code. Go to the **WebGUI** folder and read the [README](WebGUI/README.md) there.

## What's this all about?
This repository features code to make audio DSP functions available across TCP/IP networks. There are at least four reasons to do this (click on the links for details):

1. To support and simplify [cross-platform audio DSP development](docs/cross-platform-dev.md).
2. To establish a [non-proprietary, platform-independent audio plug-in technology standard](docs/network-plugin-std.md).
3. To enable [network bridging](docs/network-bridging.md)--allowing audio plug-ins and client programs such as Digital Audio Workstations (DAWs) to run on different computers (or distinct run-time environments on a single computer).
4. To allow [standard servers to be used as hardware accelerators](docs/hardware-accelerators.md) for audio DSP plug-ins.

The basic networking protocol and techniques for network bridging were developed as part of the author's [NetVST Project](http://netvst.org/wiki). In the spring of 2018, it became obvious that the NetVST plug-ins could be used to bridge arbitrary DSP code to standard DAWs without additional platform-specific adapter code, facilitating both development and use of AudioKit Core DSP code on non-Apple platforms. This led to a decision to publish all relevant code as part of a new AudioKit repo (this one) on GitHub.

## Work in progress
This repo is very much a work-in-progress, and will remain so for quite a while. What's here is little more than a proof of concept now, but I have high hopes for it in the near future!
