// knowDict is a dictionary mapping id strings to value-setting functions
// function setValue(newValue)
var knobDict = {};

// determine the desired display precision from a given "step" resolution
function getPrecisionFromStep(step) {
    if (isNaN(step)) {
        return 2;
    }

    var parts = ('' + step).split('.');
    if (parts.length > 1) {
        return parts[1].length;
    }
    else {
        return 0;
    }
}

function addKnob(containerEl, controlKey, options) { // knobColor, labelText, minValue, maxValue, initValue, power
    if (!containerEl || !controlKey || !options) {
        throw new Error('Invalid parameters for knob setup function');
    }

    // create individual DOM pieces
    var controlEl = document.createElement('div');
    controlEl.classList.add('knob-control');
    var labelEl = document.createElement('div');
    labelEl.classList.add('knob-label');
    labelEl.innerText = options.labelText || '';
    var valueEl = document.createElement('div');
    valueEl.classList.add('knob-value');
    containerEl.appendChild(controlEl);
    containerEl.appendChild(labelEl);
    containerEl.appendChild(valueEl);

    var displayPrecision = getPrecisionFromStep(options.step);
    function updateValueDisplay(newValue) {
        valueEl.innerText = Number(newValue).toFixed(displayPrecision);
    }

    // create/add input to DOM
    var knob = new PrecisionInputs.FLStandardKnob(
        controlEl,
        {
            color: options.color || PrecisionInputs.colors.blue.str,
            indicatorRingType: options.indicatorRingType || 'positive',
            min: options.power ? 0 : options.min || 0,
            max: options.power ? 1 : options.max || 1,
            initial: options.init || undefined,
            step: options.power ? 'any' : options.step || 'any'
        }
    );
    updateValueDisplay(knob.value);

    // handle input changes
    var updateValue = debounce(function(newValue) {
        if (typeof sendValueUpdate === 'function') {
            sendValueUpdate(controlKey, newValue);
        }
    }, 200);
    knob.addEventListener('change', function(evt) {
        var value = evt.target.value;
        if (options.power) {
            // this is a temporary workaround to allow non-linear interpolation on knob controls
            // once the library supports non-linear interpolation directly, we should be able to remove this
            value = options.min + (options.max - options.min) * Math.pow(value, options.power);
        }
        updateValue(value);
        updateValueDisplay(value);
    });

    // callback to update input when new values come in from server
    knobDict[controlKey] = function setValue(newValue) {
        if (options.power) {
            // this is a temporary workaround to allow non-linear interpolation on knob controls
            // once the library supports non-linear interpolation directly, we should be able to remove this
            knob.value = Math.pow((newValue - options.min) / (options.max - options.min), 1 / options.power);
        }
        else {
            knob.value = newValue;
        }
    };
}

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
}
