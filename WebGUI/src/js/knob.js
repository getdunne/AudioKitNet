// knowDict is a dictionary mapping id strings to value-setting functions
// function setValue(newValue)
var knobDict = {};



function addCaption(labelText) {
    // TODO: add caption
}

function addKnob(el, controlKey, options) { // knobColor, labelText, minValue, maxValue, initValue, taper
    if (!el || !controlKey || !options) {
        throw new Error('Invalid parameters for knob setup function');
    }

    // create/add input to DOM
    var knob = new PrecisionInputs.FLStandardKnob(el, {
        color: options.color || '#4eccff',
        indicatorRingType: options.indicatorRingType || 'positive',
        min: options.min || 0,
        max: options.max || 1,
        initial: options.init || undefined,
        step: options.step || 'any'
    });
    if (options.labelText) {
        addCaption(options.labelText);
    }

    // handle input changes
    var updateValue = debounce(function(newValue) {
        if (typeof sendValueUpdate === 'function') {
            sendValueUpdate(controlKey, newValue);
        }
    }, 200);
    knob.addEventListener('change', function(evt) {
        // TODO: set tooltip to show new value on value change
        updateValue(evt.target.value);
    });

    // callback to update input when new values come in from server
    knobDict[controlKey] = function(newValue) {
        knob.value = newValue;
    };
};

// TEMP: temporary debounce func until I update the build process
// cf. https://davidwalsh.name/javascript-debounce-function
// Returns a function, that, as long as it continues to be invoked, will not
// be triggered. The function will be called after it stops being called for
// N milliseconds. If `immediate` is passed, trigger the function on the
// leading edge, instead of the trailing.
function debounce(func, wait, immediate) {
    var timeout;
    return function() {
        var context = this, args = arguments;
        var later = function() {
            timeout = null;
            if (!immediate) func.apply(context, args);
        };
        var callNow = immediate && !timeout;
        clearTimeout(timeout);
        timeout = setTimeout(later, wait);
        if (callNow) func.apply(context, args);
    };
};
