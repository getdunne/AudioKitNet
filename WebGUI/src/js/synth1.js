if (document.getElementById('synth1-controls')) {
    var getEl = function(id) {
        return document.getElementById(id);
    };

    var purple = PrecisionInputs.colors.purple.str;
    var blue = PrecisionInputs.colors.blue.str;
    var green = PrecisionInputs.colors.green.str;
    var yellow = PrecisionInputs.colors.yellow.str;
    var red = PrecisionInputs.colors.red.str;
    var orange = PrecisionInputs.colors.orange.str;

    // oscillator 1
    addKnob(getEl('synth1-osc1-phase'), 'osc1Phases', {
        color: blue,
        labelText: 'Phase',
        min: 0, max: 10, init: 0, step: 1
    });
    addKnob(getEl('synth1-osc1-freq'), 'osc1FreqSpread', {
        color: blue,
        labelText: 'Freq',
        min: 0, max: 50, init: 0
    });
    addKnob(getEl('synth1-osc1-pan'), 'osc1PanSpread', {
        color: blue,
        labelText: 'Pan',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-osc1-tune'), 'osc1PitchOffset', {
        color: blue,
        labelText: 'Tune',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });

    // oscillator 2
    addKnob(getEl('synth1-osc2-phase'), 'osc2Phases', {
        color: green,
        labelText: 'Phase',
        min: 0, max: 10, init: 0, step: 1
    });
    addKnob(getEl('synth1-osc2-freq'), 'osc2FreqSpread', {
        color: green,
        labelText: 'Freq',
        min: 0, max: 50, init: 0
    });
    addKnob(getEl('synth1-osc2-pan'), 'osc2PanSpread', {
        color: green,
        labelText: 'Pan',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-osc2-tune'), 'osc2PitchOffset', {
        color: green,
        labelText: 'Tune',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });

    // amplitude env
    addKnob(getEl('synth1-amp-attack'), 'ampAttack', {
        color: purple,
        labelText: 'ATT', // 'Attack',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('synth1-amp-decay'), 'ampDecay', {
        color: purple,
        labelText: 'DEC', // 'Decay',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('synth1-amp-sustain'), 'ampSustain', {
        color: purple,
        labelText: 'SUS', // 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('synth1-amp-release'), 'ampRelease', {
        color: purple,
        labelText: 'REL', // 'Release',
        min: 0, max: 10, init: 0, power: 4
    });

    // filter
    addKnob(getEl('synth1-flt-amount'), 'fltEgStrength', {
        color: yellow,
        labelText: 'Amount',
        min: 0, max: 100, init: 0, power: 4
    });
    addKnob(getEl('synth1-flt-cut'), 'fltCutoff', {
        color: yellow,
        labelText: 'CUT', // 'Cutoff',
        min: 0, max: 100, init: 0, power: 4
    });
    addKnob(getEl('synth1-flt-res'), 'fltResonance', {
        color: yellow,
        labelText: 'RES', // 'Resonance',
        min: -6, max: 12, init: 0
    });
    addKnob(getEl('synth1-flt-stages'), 'fltStages', {
        color: yellow,
        labelText: 'Stages',
        min: 0, max: 4, init: 0, step: 1
    });

    // filter env
    addKnob(getEl('synth1-flt-attack'), 'fltAttack', {
        color: orange,
        labelText: 'ATT', // 'Attack',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('synth1-flt-decay'), 'fltDecay', {
        color: orange,
        labelText: 'DEC', // 'Decay',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('synth1-flt-sustain'), 'fltSustain', {
        color: orange,
        labelText: 'SUS', // 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('synth1-flt-release'), 'fltRelease', {
        color: orange,
        labelText: 'REL', // 'Release',
        min: 0, max: 10, init: 0, power: 4
    });

    // mix
    addKnob(getEl('synth1-mix-osc1'), 'osc1MixLevel', {
        color: blue,
        labelText: 'OSC1',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-osc2'), 'osc2MixLevel', {
        color: green,
        labelText: 'OSC2',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-smp1'), 'smp1MixLevel', {
        color: blue,
        labelText: 'SMP1',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-smp2'), 'smp2MixLevel', {
        color: green,
        labelText: 'SMP2',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-master'), 'masterVol', {
        color: red,
        labelText: 'Master',
        min: 0, max: 1, init: 0
    });
}
