# WebGUI

## Instant gratification
Obtain a copy of the pre-compiled binary **websocketd** executable for your platform from [websocketd.com](http://websocketd.com/) (or [get the source from GitHub](https://github.com/joewalnes/websocketd) and build it yourself), and put the executable in this folder.

Make sure to compile at least the Debug build of **DSPServer**. Then run **websocketd** as follows (this is for Windows; other platforms will be very similar):

```
websocketd --port 8080 --staticdir=. ..\DSPServer\Debug\DSPServer.exe
```

- "--port 8080" sets the port on which **websocketd** will listen. Change if you wish.
- "--staticdir=." defines this directory as where **websocketd** will look for files to serve over HTTP connections.
- The rest of the command line is the relative path to the **DSPServer** executable.

Point a web browser at ```http://localhost:8080``` and you should see the web GUI for the synthesizer implemented in **DSPServer**. (Substitute your PC's numeric IP address for "localhost" if your browser is running on a different machine.) Note you must click the **Connect** button to actually run the **DSPServer** executable, and you should remember to click it again to shut it down when you're done.

Run any suitable version of the **NetSynth plug-in** ([download from here](http://netvst.org/wiki/doku.php?id=downloading_and_using_netvst), where you'll also find detailed usage instructions) in your favorite DAW, and connect it to port 27016 on your PC. You should then be able to play and record the synthesizer.

Adjust the knobs in the web GUI, and you should hear the corresponding sound changes.

## About the web-GUI code
As I write this in late May 2018, the GUI code here is VERY rudimentary -- little more than a single SVG/Javascript-powered knob control, which isn't even very nice.

I would very much like help from anyone with better web-programming chops. In the long run I'd like to see a fairly complete set of web-hosted audio controls, which look nice both individually and in combination, as well as a variety of example web GUIs showing how to assemble them in groups. (With luck, the PaintCode files from the [AudioKitGraphics repo](https://github.com/AudioKit/AudioKitGraphics) may be directly usable. Fingers crossed!)
