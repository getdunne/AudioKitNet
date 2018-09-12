if (document.getElementById('synth1-controls')) {
    var getEl = function(id) {
        return document.getElementById(id);
    };

    var colors = {
        purple: '#8b63ff',
        blue: '#4eccff',
        green: '#83eb42',
        yellow: '#f5cc47',
        red: '#ff4e60',
        orange: '#ffa830'
    };

    // oscillator 1
    addKnob(getEl('synth1-osc1-phase'), 'osc1Phases', {
        color: colors.blue,
        labelText: 'Phase',
        min: 0, max: 10, init: 0, step: 1
    });
    addKnob(getEl('synth1-osc1-freq'), 'osc1FreqSpread', {
        color: colors.blue,
        labelText: 'Freq',
        min: 0, max: 50, init: 0
    });
    addKnob(getEl('synth1-osc1-pan'), 'osc1PanSpread', {
        color: colors.blue,
        labelText: 'Pan',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-osc1-tune'), 'osc1PitchOffset', {
        color: colors.blue,
        labelText: 'Tune',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });

    // oscillator 2
    addKnob(getEl('synth1-osc2-phase'), 'osc2Phases', {
        color: colors.green,
        labelText: 'Phase',
        min: 0, max: 10, init: 0, step: 1
    });
    addKnob(getEl('synth1-osc2-freq'), 'osc2FreqSpread', {
        color: colors.green,
        labelText: 'Freq',
        min: 0, max: 50, init: 0
    });
    addKnob(getEl('synth1-osc2-pan'), 'osc2PanSpread', {
        color: colors.green,
        labelText: 'Pan',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-osc2-tune'), 'osc2PitchOffset', {
        color: colors.green,
        labelText: 'Tune',
        indicatorRingType: 'split',
        min: -24, max: 24, init: 0, step: 1
    });

    // amplitude env
    addKnob(getEl('synth1-amp-attack'), 'ampAttack', {
        color: colors.purple,
        labelText: 'Attack',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-amp-decay'), 'ampDecay', {
        color: colors.purple,
        labelText: 'Decay',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-amp-sustain'), 'ampSustain', {
        color: colors.purple,
        labelText: 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('synth1-amp-release'), 'ampRelease', {
        color: colors.purple,
        labelText: 'Release',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });

    // filter
    addKnob(getEl('synth1-flt-cut'), 'fltCutoff', {
        color: colors.yellow,
        labelText: 'Cutoff',
        min: 0, max: 100, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-flt-res'), 'fltResonance', {
        color: colors.yellow,
        labelText: 'Resonance',
        min: -6, max: 12, init: 0
    });
    addKnob(getEl('synth1-flt-stages'), 'fltStages', {
        color: colors.yellow,
        labelText: 'Stages',
        min: 0, max: 4, init: 0, step: 1
    });

    // filter env
    addKnob(getEl('synth1-flt-attack'), 'fltAttack', {
        color: colors.orange,
        labelText: 'Attack',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-flt-decay'), 'fltDecay', {
        color: colors.orange,
        labelText: 'Decay',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-flt-sustain'), 'fltSustain', {
        color: colors.orange,
        labelText: 'Sustain',
        min: 0, max: 1.0, init: 0
    });
    addKnob(getEl('synth1-flt-release'), 'fltRelease', {
        color: colors.orange,
        labelText: 'Release',
        min: 0, max: 10, init: 0, taper: 4 // TODO: exponential!
    });
    addKnob(getEl('synth1-flt-amount'), 'fltEgStrength', {
        color: colors.orange,
        labelText: 'Amount',
        min: 0, max: 100, init: 0, taper: 4 // TODO: exponential!
    });

    // mix
    addKnob(getEl('synth1-mix-osc1'), 'osc1MixLevel', {
        color: colors.blue,
        labelText: 'OSC1',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-osc2'), 'osc2MixLevel', {
        color: colors.green,
        labelText: 'OSC2',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-smp1'), 'smp1MixLevel', {
        color: colors.blue,
        labelText: 'SMP1',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-smp2'), 'smp2MixLevel', {
        color: colors.green,
        labelText: 'SMP2',
        min: 0, max: 1, init: 0
    });
    addKnob(getEl('synth1-mix-master'), 'masterVol', {
        color: colors.red,
        labelText: 'Master',
        min: 0, max: 1, init: 0
    });
}
