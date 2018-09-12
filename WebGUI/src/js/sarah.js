if (document.getElementById('sarah-controls')) {
    // set up knobs
    addKnob('sarah-osc1PitchOffset', 'osc1PitchOffset', 'lightBlue', 'P.Offset', -24, 24, 0, 0);

    addKnob('sarah-osc2PitchOffset', 'osc2PitchOffset', 'lightBlue', 'P.Offset', -24, 24, 0, 0);

    addKnob('sarah-ampAttack', 'ampAttack', 'red', 'Attack', 0, 10, 0, 4);
    addKnob('sarah-ampDecay', 'ampDecay', 'lightBlue', 'Decay', 0, 10, 0, 4);
    addKnob('sarah-ampSustain', 'ampSustain', 'green', 'Sustain', 0, 1.0, 0, 1);
    addKnob('sarah-ampRelease', 'ampRelease', 'yellow', 'Release', 0, 10, 0, 4);

    addKnob('sarah-fltAttack', 'fltAttack', 'darkRed', 'Attack', 0, 10, 0, 4);
    addKnob('sarah-fltDecay', 'fltDecay', 'Blue', 'Decay', 0, 10, 0, 4);
    addKnob('sarah-fltSustain', 'fltSustain', 'darkGreen', 'Sustain', 0, 1.0, 0, 1);
    addKnob('sarah-fltRelease', 'fltRelease', 'coral', 'Release', 0, 10, 0, 4);
    addKnob('sarah-fltEgStrength', 'fltEgStrength', 'lightGray', 'Envelope', 0, 100, 0, 4);
    addKnob('sarah-fltCutoff', 'fltCutoff', 'lightGray', 'Cutoff', 0, 100, 0, 4);
    addKnob('sarah-fltResonance', 'fltResonance', 'lightGray', 'Res.dB', -6, 12, 0, 1);

    addKnob('sarah-osc1MixLevel', 'osc1MixLevel', 'lightBlue', 'Osc1', 0, 1, 0, 1);
    addKnob('sarah-osc2MixLevel', 'osc2MixLevel', 'lightBlue', 'Osc2', 0, 1, 0, 1);
    addKnob('sarah-masterVol', 'masterVol', 'lightGray', 'Master', 0, 1, 0, 1);
}
