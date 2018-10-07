if (document.getElementById('sarah-controls')) {
    var getEl = function(id) {
        return document.getElementById(id);
    };

    var purple = PrecisionInputs.colors.purple.str;
    var blue = PrecisionInputs.colors.blue.str;
    var green = PrecisionInputs.colors.green.str;
    var yellow = PrecisionInputs.colors.yellow.str;
    var red = PrecisionInputs.colors.red.str;
    var orange = PrecisionInputs.colors.orange.str;

    // tune
    addKnob(getEl('sarah-osc1-tune'), 'osc1PitchOffset', {
        color: blue,
        labelText: 'OSC1',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });
    addKnob(getEl('sarah-osc2-tune'), 'osc2PitchOffset', {
        color: green,
        labelText: 'OSC2',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });

    // amplitude env
    addKnob(getEl('sarah-amp-attack'), 'ampAttack', {
        color: purple,
        labelText: 'ATT', // 'Attack',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('sarah-amp-decay'), 'ampDecay', {
        color: purple,
        labelText: 'DEC', // 'Decay',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('sarah-amp-sustain'), 'ampSustain', {
        color: purple,
        labelText: 'SUS', // 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('sarah-amp-release'), 'ampRelease', {
        color: purple,
        labelText: 'REL', // 'Release',
        min: 0, max: 10, init: 0, power: 4
    });

    // filter
    addKnob(getEl('sarah-flt-amount'), 'fltEgStrength', {
        color: yellow,
        labelText: 'Amount',
        min: 0, max: 100, init: 0, power: 4
    });
    addKnob(getEl('sarah-flt-cut'), 'fltCutoff', {
        color: yellow,
        labelText: 'CUT', // 'Cutoff',
        min: 0, max: 100, init: 0, power: 4
    });
    addKnob(getEl('sarah-flt-res'), 'fltResonance', {
        color: yellow,
        labelText: 'RES', // 'Resonance',
        min: -6, max: 12, init: 0
    });

    // filter env
    addKnob(getEl('sarah-flt-attack'), 'fltAttack', {
        color: orange,
        labelText: 'ATT', // 'Attack',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('sarah-flt-decay'), 'fltDecay', {
        color: orange,
        labelText: 'DEC', // 'Decay',
        min: 0, max: 10, init: 0, power: 4
    });
    addKnob(getEl('sarah-flt-sustain'), 'fltSustain', {
        color: orange,
        labelText: 'SUS', // 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('sarah-flt-release'), 'fltRelease', {
        color: orange,
        labelText: 'REL', // 'Release',
        min: 0, max: 10, init: 0, power: 4
    });

    // mix
    addKnob(getEl('sarah-mix-osc1'), 'osc1MixLevel', {
        color: blue,
        labelText: 'Osc1',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('sarah-mix-osc2'), 'osc2MixLevel', {
        color: green,
        labelText: 'Osc2',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('sarah-mix-master'), 'masterVol', {
        color: red,
        labelText: 'Master',
        min: 0, max: 1, init: 0
    });
}
