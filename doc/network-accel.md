# Servers as hardware accelerators for DSP plug-ins

Desktop computer performance no longer increases by leaps and bounds each year, as it did prior to 2008. To achieve substantial gains in DSP performance, plug-in vendors will soon have to consider *hardware acceleration*. This page argues that this is best done using off-the-shelf servers with [network bridging](network-bridging.md).

## Desktop performance has stalled
Prior to 2008, when Intel introduced the first [Core-series microprocessors](https://en.wikipedia.org/wiki/Intel_Core)), desktop PC performance increased by leaps and bounds each year. Companies routinely replaced machines at least every 3 years, consumers as often as every year. All this frenzied buying helped keep PC prices extremely low.

All this changed dramatically in 2010:
- Standard process-engineering progress kept the party going for another 2-3 years
- After that, performance gains proved increasingly difficult--and hugely *expensive*
- As a result, prices of high-performance PCs rose dramatically, so people bought less, so prices rose even more.
- Apple introduced the iPad, and millions of consumers realized they didn't actually need a desktop PC anymore, so sales dropped, so prices rose again.
- The new Internet-powered industry of "data centers" became the big driver of the high-performance microprocessor market.

In short, the economics of desktop PCs collapsed, and all the "action" in the microprocessor business quickly shifted to servers at the high end, and to the shiny new "mobile devices" (smartphones and tablets) at the low end. Today, if you want a high-performance computer, you either buy a server, or a [desktop computer built with server components](https://en.wikipedia.org/wiki/IMac_Pro).

## Audio DSP is a CPU-hungry niche market
In a [2017 interview](https://youtu.be/wK-jQty9rrQ#t=03m30s), Urs Heckmann of U-He talked about how the design of software synthesizers is mainly based on compromises one has to make, to reduce CPU load. Among high-end plug-in developers, code optimization has already yielded as much performance as can be expected. They will therefore soon be forced to consider some kind of hardware acceleration, to create the next generations of advanced synthesis and signal-processing plug-ins.

At the time of writing, [Universal Audio](https://www.uaudio.com/) is the only plug-in vendor making substantial use of hardware acceleration, manufacturing their own very non-standard hardware based on [SHARC DSP chips](https://en.wikipedia.org/wiki/Super_Harvard_Architecture_Single-Chip_Computer), but this is a very tough road--the electronics business is entirely volume-driven, and one small manufacturer cannot possibly achieve the volume of sales necessary to have any control over prices and supply-chain variables.

## Servers are where the growth is
Large data centers are driving the high-performance computer market, and they are far more hungry for CPU power than the audio business--and rich enough to satisfy their hunger. A tsunami of used/refurbished servers is about to hit the computer market, as large data-center operators like Amazon begin to modernize huge swaths of their operations.

If the goals of this project can be realized, recording studios--even small ones--will be well positioned to benefit by installing racks of these cheap second-hand servers as DSP accelerators, and plug-in vendors who offer [network-based bridging](network-bridging.md) as an acceleration option will enjoy higher sales than their rivals who do not.

Of course, this is a classic chicken-and-egg problem--studio owners won't buy servers until there is software available to take advantage of them, and plug-in vendors won't offer server support until servers are in widespread use--but sooner or later, something is going to give.

For a possibly-interesting take on some of the market aspects, see [commercial deployment of NetVST-like technologies](http://netvst.org/wiki/doku.php?id=commercial_deployment_of_netvst-like_technologies).

## One possible scenario
Berlin-based [Native Instruments GmbH](https://www.native-instruments.com/en/) is the 800-pound gorilla among plug-in vendors. They already offer their giant [Komplete 11 Ultimate](https://www.native-instruments.com/en/products/komplete/bundles/komplete-11-ultimate/) package in a hardware disk format. They might, some day, consider offering a "Komplete server edition" package, with network acceleration added to the most CPU-intensive plug-ins (basically just [Kontakt 5](https://www.native-instruments.com/en/products/komplete/samplers/kontakt-5/), [Reaktor 6](https://www.native-instruments.com/en/products/komplete/synths/reaktor-6/), and their Reaktor-based synths). That alone would kick-start the studio market for servers, *provided the technology is open enough that other vendors can benefit*. Any attempt to build a closed system is likely to be self-defeating.
