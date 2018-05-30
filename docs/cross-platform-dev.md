# Cross-platform audio DSP development

The **AudioKit-net** code repository was created to address two problems of developing new audio DSP code using **AudioKit**:

1. The large amount of "wrapper" code to make DSP code accessible in an AudioKit application.
2. The lack of a simple way to make GUIs for adjusting DSP parameters during testing.

## Avoiding DSP wrapper code
AudioKit is a great library for building audio *applications* by connecting *finished, debugged DSP code modules* called *nodes*, but provides little support for developing those nodes from scratch.

AudioKit is built around the *Audio Units* interface specification supported in macOS and iOS, so the first step in bringing new DSP code into AudioKit (typically written in C or C++) is to add *three layers* of "wrapper" code (one in Objective-C, a second in either Objective-C or Swift, and a third in Swift). This is not only a lot of work; it also makes what is most likely platform-independent code into entirely Apple-specific code, which can only run on Apple platforms and requires Xcode (and a Mac to run it on) as the development environment.

The [JUCE](https://juce.com/) framework provides a set of mature, stable interface layers which effectively bridge new DSP code to multiple proprietary plug-in interfaces, plus very good support for writing cross-platform GUIs. However, it does this by introducing *yet another set of proprietary APIs* which are *large*, *complex*, and *poorly documented*, which equates to a huge learning curve for the aspiring DSP code developer.

In contrast, the **AudioKit-net** approach is to write DSP code in C++, to conform to a minimalist, platform-independent interface:

```c++
    void acceptMidi(MIDIMessageInfoStruct* pMidi, int nMessages);
    void acceptParamChanges(ParamMessageStruct* pMsgs, int nMessages);
    bool command(char* cmd);
    void render(float** buffers, int nFrames);
```

Already-written **AudioKit-net** code can "wrap" any such DSP object as a "DSP daemon" program, which listens on two TCP/IP network ports--one ordinary socket for the real-time audio-rendering thread, and one web socket to support web-based GUIs. Other pre-written code connects this daemon to audio and MIDI hardware, in any of three different ways:

1. **NetVST plug-ins** can be used in any DAW host, either on the same computer or a different one, bridging the plug-in interface (AU or VST) to the network protocol.
2. **AudioKit applications** can make use of what amounts to the NetVST AU plug-in, re-wrapped as an Audio Unit and an AudioKit "node" class.
3. **JUCE applications and plug-ins** can use a similar technique, making use of a C++ proxy object instead of the actual DSP code.

Once the new DSP code is finished and tested to the programmer's satisfaction, it can then be wrapped as necessary to become a standard AudioKit node class.

Note that a JUCE plug-in (case 3) can be written to allow the user the option to choose, at run-time, whether to run the DSP code either directly (locally) or with network bridging to a "DSP daemon" running on a server, which then acts as a [hardware accelerator](hardware-accelerators.md) to augment the CPU power of the DAW/host machine.

## Simplified GUIs
Modern web technologies, specifically HTML5, Scalable Vector Graphics (SVG), JavaScript, and WebSockets, provide all that is needed to create simple GUIs which can be hosted in any web browser. Familiar audio controls such as knobs, buttons and switches can be built, and used to generate simple text commands like "ampEGAttackSeconds=0.1" which can be sent across an open web socket, to emerge within the AudioKit-net DSP daemon program as the arguments of calls to the DSP module's `command()` function.

Clearly, the hardest part of this is development of the SVG and JavaScript for common controls, and the HTML/CSS code needed to combine these into pleasing and useful layouts. The community of "Web Synth" developers (see e.g. http://www.websynths.org/) has already developed substantial amounts of such GUI code, and some general web developers have also published code for useful GUI components. It is hoped that over time, interested collaborators will help to create a curated library of such GUI components under the AudioKit-net umbrella.

Creating GUIs using web technologies has many advantages:

1. It is inherently operating system-independent
2. It provides a straightforward way to allow DSP code to run on a fully headless server ("DSP accelerator"), while presenting a nice interactive GUI on whatever computer is used by the musician.
3. Handling of multiple DSP instances, multiple web GUI sessions, etc. is straightforward.
4. Together with the basic real-time socket protocol used for real-time audio, MIDI and parameter changes, it provides a basis for a new, [non-proprietary, platform-independent audio plug-in technology standard](network-plugin-std.md).

Here is an example of a working web GUI for a new synthesizer based on new AudioKitCore code, which will remain in this repo until it's further fleshed-out and tested, then moved to the [main AudioKit repo](https://github.com/AudioKit/AudioKit). (Note not all synth parameters are available from this GUI yet.)

![web-gui-1.png](web-gui-1.png)

