# NetVST_Windows
This is the source code for the **NetFilter** and **NetSynth** VST (v2) plug-ins from the [NetVST project](http://netvst.org/wiki). It is mainly included here for reference. (The code for an experimental third plug-in called **NetWrapper** is also included; see [this post on the NetVST blog](http://netvst.org/blog/2017/08/vst-gui-on-one-pc-dsp-on-another).)

The **win** folder contains the project files, which are for MS Visual Studio 2010; these can easily be upgraded for later versions of Visual Studio. In order to compile the code, however, you will need the **VST 2.4 SDK** from Steinberg (*VST* is a registered trade mark of Steinberg GmbH, Berlin).

You can download the newer **VST 3.x SDK** from [Steinberg's developer web site](https://www.steinberg.net/en/company/developers.html), but in order to use it for anything other than personal exploration (i.e., to distribute any VST-compatible plug-ins you create), you have to obtain a signed license from Steinberg. This is straightforward and *free*, and Steinberg's license terms are very reasonable.

**The VST 2.4 SDK will be unavailable after September 2018!** As I write this (late May 2018), the essential parts of the VST 2.4 SDK are available as a subdirectory within the newer/larger 3.6.9 SDK. However, this will no longer be the case after the end of September. See [this page](http://www.synthanatomy.com/2018/05/plugin-format-vst-2-coming-end.html) for a copy of the official statement from Steinberg.

The important point is simply to publish this source code, in order to document the VST host-interface and network-interface aspects. The code itself isn't special in any way, and it should be simple to build functionally-identical plug-ins using more modern APIs (including [JUCE](https://www.juce.com)) for any desired target platform.

There is only one aspects of this code which is even slightly non-obvious: The *Render()* routine sends exactly one network packet for each host-supplied audio buffer, and receives exactly one packet in response. This helps to minimize network delays.

**To obtain pre-built binaries of all NetVST tools, go to the [NetVST site](http://netvst.org/wiki/doku.php?id=downloading_and_using_netvst)**.

## What about NetVSTHost?
The centerpiece of the [NetVST project](http://netvst.org/wiki) is a network-capable VST host program called **NetVSTHost**. You may be wondering why its source-code is not included here. This is a matter of licensing terms, as follows.

**NetVSTHost** is essentially a rebuilt version of Hermann Seib's very popular free [VSTHost](http://hermannseib.com/English/vsthost.htm) program. The code was based on Hermann's "open-source variant"--basically an older version whose source-code Hermann had published under the terms of the [GNU Lesser GPL](http://www.gnu.org/copyleft/lesser.html), which you can still download freely from his web site.

Actually publishing the **NetVSTHost** code as open-source is problematic for several reasons, as follows:
1. I would only do so with Hermann's explicit permission, but I have been unable to contact him for over a year; I don't know why.
2. It is not clear that the [GNU Lesser GPL](http://www.gnu.org/copyleft/lesser.html) truly applies to the kind of combined work I created based on Hermann's code. At the very least, it might be legally complex. At worst, the full [General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html) may apply instead, which would force my new code--for which I prefer the far more permissive [MIT License](https://opensource.org/licenses/MIT)--to be subject to the GPL, complicating any future commercial use.
3. The source-code is incomplete without both the VST 2.4 SDK and the ASIO SDK (also from Steinberg), neither of which is freely licensed, and (see above), the former of which is about to become unavailable forever, so it's not possible to publish a truly complete open-source version.

All of the important code from **NetVSTHost** has been transformed into **DSPServer** and is available here in this repo, under the [MIT License](https://opensource.org/licenses/MIT). Based on this, it should be very straightforward to create a functionally-equivalent replacement for **NetVSTHost** using more modern API's, e.g. [JUCE](https://www.juce.com). JUCE's licensing terms require that open-source publishing be subject to the GNU [General Public License](https://www.gnu.org/licenses/gpl-3.0.en.html), **BUT** since the relevant aspects have already been published here under the MIT license, this should no longer be an issue.
