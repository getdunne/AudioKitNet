if (document.getElementById('synth1-controls')) {
    // set up knobs
    addKnob('synth1-osc1Phases', 'osc1Phases', 'lightBlue', 'Phases', 0, 10, 0, 0);
    addKnob('synth1-osc1FreqSpread', 'osc1FreqSpread', 'lightBlue', 'Freq.Sprd', 0, 50, 0, 1);
    addKnob('synth1-osc1PanSpread', 'osc1PanSpread', 'lightBlue', 'Pan.Sprd', 0, 1, 0, 1);
    addKnob('synth1-osc1PitchOffset', 'osc1PitchOffset', 'lightBlue', 'P.Offset', -24, 24, 0, 0);

    addKnob('synth1-osc2Phases', 'osc2Phases', 'lightBlue', 'Phases', 0, 10, 0, 0);
    addKnob('synth1-osc2FreqSpread', 'osc2FreqSpread', 'lightBlue', 'Freq.Sprd', 0, 50, 0, 1);
    addKnob('synth1-osc2PanSpread', 'osc2PanSpread', 'lightBlue', 'Pan.Sprd', 0, 1, 0, 1);
    addKnob('synth1-osc2PitchOffset', 'osc2PitchOffset', 'lightBlue', 'P.Offset', -24, 24, 0, 0);

    addKnob('synth1-ampAttack', 'ampAttack', 'red', 'Attack', 0, 10, 0, 4);
    addKnob('synth1-ampDecay', 'ampDecay', 'lightBlue', 'Decay', 0, 10, 0, 4);
    addKnob('synth1-ampSustain', 'ampSustain', 'green', 'Sustain', 0, 1.0, 0, 1);
    addKnob('synth1-ampRelease', 'ampRelease', 'yellow', 'Release', 0, 10, 0, 4);

    addKnob('synth1-fltAttack', 'fltAttack', 'darkRed', 'Attack', 0, 10, 0, 4);
    addKnob('synth1-fltDecay', 'fltDecay', 'Blue', 'Decay', 0, 10, 0, 4);
    addKnob('synth1-fltSustain', 'fltSustain', 'darkGreen', 'Sustain', 0, 1.0, 0, 1);
    addKnob('synth1-fltRelease', 'fltRelease', 'coral', 'Release', 0, 10, 0, 4);
    addKnob('synth1-fltEgStrength', 'fltEgStrength', 'lightGray', 'Envelope', 0, 100, 0, 4);
    addKnob('synth1-fltCutoff', 'fltCutoff', 'lightGray', 'Cutoff', 0, 100, 0, 4);
    addKnob('synth1-fltResonance', 'fltResonance', 'lightGray', 'Res.dB', -6, 12, 0, 1);
    addKnob('synth1-fltStages', 'fltStages', 'lightGray', 'Stages', 0, 4, 0, 0);

    addKnob('synth1-osc1MixLevel', 'osc1MixLevel', 'lightBlue', 'Osc1', 0, 1, 0, 1);
    addKnob('synth1-osc2MixLevel', 'osc2MixLevel', 'lightBlue', 'Osc2', 0, 1, 0, 1);
    addKnob('synth1-smp1MixLevel', 'smp1MixLevel', 'lightBlue', 'Smp1', 0, 1, 0, 1);
    addKnob('synth1-smp2MixLevel', 'smp2MixLevel', 'lightBlue', 'Smp2', 0, 1, 0, 1);
    addKnob('synth1-masterVol', 'masterVol', 'lightGray', 'Master', 0, 1, 0, 1);
}
