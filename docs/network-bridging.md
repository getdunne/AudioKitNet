# Network bridging for audio plug-ins
There are many different types of audio plug-ins, and not all host programs support all types. This has spurred the development of various software "bridging" solutions. There are at least five reasons why bridging may be required:
1. To allow a host which can only directly load plug-ins of type *X* to access plug-ins of some other type *Y*. This was the motivation behind now-discontinued products from e.g. [FXpansion](https://www.fxpansion.com/) (now part of [Roli, Inc.](https://roli.com/)), among others.
2. To allow a 32-bit host to load 64-bit plug-ins, or vice versa. This is addressed by e.g. [jBridge](https://jstuff.wordpress.com/) and [Metaplugin](https://ddmf.eu/metaplugin-chainer-vst-au-rtas-aax-wrapper/).
3. To allow a host program running on one program to access plug-ins running on a different computer, to harness additional CPU power. Examples of this include [Vienna Ensemble Pro](https://www.vsl.co.at/en/Vienna_Software_Package/Vienna_Ensemble_PRO), [NetVST](http://netvst.org/wiki), and [DAWconnect](https://www.dawconnect.com/).
4. To allow plug-ins built for operating sytem *A* to be accessed from a host program running under a different operating system *B*. This is most commonly implemented using the same techniques as #3.
5. To allow older plug-ins to be used with a newer, incompatible host.

## Legacy plug-ins
There are literally thousands of plug-ins available, often at little or no cost, which the original vendor is no longer able to update to run with the latest host programs. The original vendor may have gone out of business (or died). Or, the vendor may simply be too small or ill-equipped to muster the necessary programming effort. Or, perhaps most tragically, the plug-ins were built with a legacy software tool like [SynthEdit](http://www.synthedit.com/), using legacy libraries which are no longer maintained and thus unavailable in modern versions.

This project's predecessor [NetVST](http://netvst.org/wiki) was motivated entirely by a desire to make legacy SynthEdit plugins--all available only for 32-bit Windows--usable on newer 64-bit Windows and Macintosh computers.

## Multi-computer setups
[Vienna Symphonic Library GmbH](https://www.vsl.co.at/en) is a well-established European provider of orchestral sample libraries. Because these tend to be extremely large (hundreds of gigabytes), and because most of their users are film and television score composers who need to have an entire orchestra's worth of samples available simultaneously, they developed the [Vienna Ensemble Pro](https://www.vsl.co.at/en/Vienna_Software_Package/Vienna_Ensemble_PRO) software (VEP) to allow a single DAW to access plug-ins (and entire signal-processing chains) running on one or even many other computers, connected by a high-speed LAN.

Apple's popular [Logic Pro](https://en.wikipedia.org/wiki/Logic_Pro) DAW has had built-in networking very similar to that of VEP for many years. Unfortunately, this is subject to so many restrictions (all computers must be Macs, all running the latest OS, etc.) that few people ever use this remarkable capability.

## The "Remote GUI" problem
A multi-computer setup is not very useful if every computer needs its own monitor, keyboard and mouse, and if all this hardware must be within reach of the operator. Network bridging solutions such like VEP and NetVST are thus of limited use, because they don't provide any way to make target plugins' GUIs available on the screen of the DAW computer.

DAWconnect's *Studio Server* attempts to address this problem using an off-the-shelf remote-UI technology ([VMWare Horizon](https://www.vmware.com/ca/products/horizon.html) or similar), but such high-bandwidth systems tend to compromise LAN latency.

As part of the NetVST Project, the author conducted an [experiment](https://www.youtube.com/watch?v=AuBSN4kmIyI&t=5s) wherein an off-the-shelf plug-in was run simultaneously on two computers, so the GUI could be presented on the DAW-host computer, while the CPU-intensive DSP ran on the other one. This was achieved by running the "local" plug-in hosted by a "wrapper" plug-in, which intercepted GUI-initiated parameter changes and routed these to the "remote" plug-in, using the same mechanism in the NetVST protocol used for host automation of parameter changes. This worked reasonably well for the test plugin ([AAS StringStudio](https://www.applied-acoustics.com/string-studio-vs-2/)), but would not be adequate for more sophisticated plug-ins like [Spectrasonics' Omnisphere 2](https://www.spectrasonics.net/products/omnisphere/) which does not expose all of its parameters for DAW automation.

The ultimate solution will be to write plug-ins with a user-selectable option to run their DSP operations on a remote server, set up as a *DSP service process* (*daemon*) as this AudioKit-net project demonstrates. Plug-in vendors will be motivated to do this, as it appears to be the only cost-effective way to achieve [hardware acceleration](hardware-accelerators.md).

## Headless servers
Once the "remote GUI problem" is solved, the way is clear to run DSP daemons on racks of "headless" servers, having no need for monitors, keyboards, etc. Workstation operating systems such as Windows don't properly support headless operation, but server OSs like Linux, the BSDs, and Solaris do.

Very few plug-ins actually require the unique resources of Windows or macOS just to run their DSP code. These are only needed for DAWs, which have to connect with low latency to MIDI and audio I/O hardware. At most, DSP code will require a good file system, which is available on all OSs. Moreover, server OSs (including Linux) make very good file servers, so in a networked environment, it will be possible in principle to have only one central copy of large file sets such as sample libraries. (In practice, multiple copies will still be needed to improve performance, and avoid compromising network latency.)
