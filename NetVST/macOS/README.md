# NetVST_MacOS
This is the source code for the **NetFilter** and **NetSynth** Audio Unit (AUv2) plug-ins from the [NetVST project](http://netvst.org/wiki). It is mainly included here for reference; the code, and especially the *.xcodeproj* files, are intended for use with a MUCH older version of Xcode, from way back when Apple actually included the Audio Units SDK with the IDE.

This code can also be compiled under the latest Xcode 9.x, but obtaining and setting up the necessary Audio Units (version 2) SDK is not trivial. Someday if I'm feeling adventurous, I may add details here about how to do this.

The important point is simply to publish this source code, in order to document the AU host-interface and network-interface aspects. The code itself isn't special in any way, and it should be simple to build functionally-identical plug-ins using more modern APIs (including [JUCE](https://www.juce.com)) for any desired target platform.

There is only one aspects of this code which is even slightly non-obvious: The *Render()* routine sends exactly one network packet for each host-supplied audio buffer, and receives exactly one packet in response. This helps to minimize network delays.

**To obtain pre-built binaries of all NetVST tools, go to the [NetVST site](http://netvst.org/wiki/doku.php?id=downloading_and_using_netvst)**.
