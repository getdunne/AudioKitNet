// set up knobs
makeKnob("osc1Phases", "lightBlue", "Phases", 0, 10, 0, 0, sendValueUpdate);
makeKnob("osc1FreqSpread", "lightBlue", "Freq.Sprd", 0, 50, 0, 1, sendValueUpdate);
makeKnob("osc1PanSpread", "lightBlue", "Pan.Sprd", 0, 1, 0, 1, sendValueUpdate);
makeKnob("osc1PitchOffset", "lightBlue", "P.Offset", -24, 24, 0, 0, sendValueUpdate);

makeKnob("osc2Phases", "lightBlue", "Phases", 0, 10, 0, 0, sendValueUpdate);
makeKnob("osc2FreqSpread", "lightBlue", "Freq.Sprd", 0, 50, 0, 1, sendValueUpdate);
makeKnob("osc2PanSpread", "lightBlue", "Pan.Sprd", 0, 1, 0, 1, sendValueUpdate);
makeKnob("osc2PitchOffset", "lightBlue", "P.Offset", -24, 24, 0, 0, sendValueUpdate);

makeKnob("ampAttack", "red", "Attack", 0, 10, 0, 4, sendValueUpdate);
makeKnob("ampDecay", "lightBlue", "Decay", 0, 10, 0, 4, sendValueUpdate);
makeKnob("ampSustain", "green", "Sustain", 0, 1.0, 0, 1, sendValueUpdate);
makeKnob("ampRelease", "yellow", "Release", 0, 10, 0, 4, sendValueUpdate);

makeKnob("fltAttack", "darkRed", "Attack", 0, 10, 0, 4, sendValueUpdate);
makeKnob("fltDecay", "Blue", "Decay", 0, 10, 0, 4, sendValueUpdate);
makeKnob("fltSustain", "darkGreen", "Sustain", 0, 1.0, 0, 1, sendValueUpdate);
makeKnob("fltRelease", "coral", "Release", 0, 10, 0, 4, sendValueUpdate);
makeKnob("fltEgStrength", "lightGray", "Envelope", 0, 100, 0, 4, sendValueUpdate);
makeKnob("fltCutoff", "lightGray", "Cutoff", 0, 100, 0, 4, sendValueUpdate);
makeKnob("fltResonance", "lightGray", "Res.dB", -6, 12, 0, 1, sendValueUpdate);
makeKnob("fltStages", "lightGray", "Stages", 0, 4, 0, 0, sendValueUpdate);

makeKnob("osc1MixLevel", "lightBlue", "Osc1", 0, 1, 0, 1, sendValueUpdate);
makeKnob("osc2MixLevel", "lightBlue", "Osc2", 0, 1, 0, 1, sendValueUpdate);
makeKnob("smp1MixLevel", "lightBlue", "Smp1", 0, 1, 0, 1, sendValueUpdate);
makeKnob("smp2MixLevel", "lightBlue", "Smp2", 0, 1, 0, 1, sendValueUpdate);
makeKnob("masterVol", "lightGray", "Master", 0, 1, 0, 1, sendValueUpdate);
