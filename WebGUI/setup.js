// set up knobs
makeKnob("osc1Phases", "lightGray", "Phases", 0, 10, 1, 0, sendValueUpdate);
makeKnob("osc2PitchOffset", "lightGray", "P.Offset", -24, 24, 0, 0, sendValueUpdate);

makeKnob("ampAttack", "red", "Attack", 0, 10, 0.1, 4);
makeKnob("ampDecay", "lightBlue", "Decay", 0, 10, 0.1, 4);
makeKnob("ampSustain", "green", "Sustain", 0, 1.0, 0.8, 1);
makeKnob("ampRelease", "yellow", "Release", 0, 10, 0.5, 4);

makeKnob("fltAttack", "darkRed", "Attack", 0, 10, 0.1, 4);
makeKnob("fltDecay", "Blue", "Decay", 0, 10, 0.1, 4);
makeKnob("fltSustain", "darkGreen", "Sustain", 0, 1.0, 0.8, 1);
makeKnob("fltRelease", "coral", "Release", 0, 10, 0.5, 4);
