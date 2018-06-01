// knowDict is a dictionary mapping id strings to value-setting functions
// function setValue(newValue)
var knobDict = {};

function makeDial(knobColor) {
    var group = document.createElementNS("http://www.w3.org/2000/svg", "g");
    group.setAttribute("transform", "translate(35, 35) rotate(0 0 0) translate(-35, -35)");
    var circle = document.createElementNS("http://www.w3.org/2000/svg", "circle");
    circle.setAttribute("cx", "35");
    circle.setAttribute("cy", "35");
    circle.setAttribute("r", "30");
    circle.setAttribute("stroke", "black");
    circle.setAttribute("stroke-width", "2");
    circle.setAttribute("fill", knobColor);
    group.appendChild(circle);
    var line = document.createElementNS("http://www.w3.org/2000/svg", "line");
    line.setAttribute("x1", "35");
    line.setAttribute("x2", "35");
    line.setAttribute("y1", "30");
    line.setAttribute("y2", "6");
    line.setAttribute("stroke", "black");
    line.setAttribute("stroke-width", "2");
    group.appendChild(line);
    return group;
}

function makeCaption(labelText) {
    var textObj = document.createElementNS("http://www.w3.org/2000/svg", "text");
    textObj.setAttribute("x", "35");
    textObj.setAttribute("y", "85");
    textObj.setAttribute("text-anchor", "middle");
    textObj.textContent = labelText;
    return textObj;
}

function makeKnob(id, knobColor, labelText, minValue, maxValue, initValue, taper, valueSetFunction) {
    var svg = document.getElementById(id);
    svg.setAttribute("width", "70");
    svg.setAttribute("height", "100");
    var dial = makeDial(knobColor);
    var caption = makeCaption(labelText);
    svg.appendChild(dial);
    svg.appendChild(caption);
    
    var label = labelText;
    var min = minValue;
    var max = maxValue;
    var power = taper;
    var value = initValue;
    setValue(initValue);
    
    var fraction = (value - min) / (max - min)
    if (power != 0) fraction = Math.pow(fraction, 1.0/power);
    var rotAngle = -160 + 320 * fraction;
    dial.setAttribute("transform", "translate(35, 35) rotate(" + String(rotAngle) + " 0 0) translate(-35, -35)")
    
    dial.addEventListener('mousedown', click, {passive: true});
    dial.addEventListener('mouseover', hover, {passive: true});
    dial.addEventListener('mouseleave', leave, {passive: true});
    document.addEventListener('mousemove', drag);
    document.addEventListener('mouseup', release, {passive: true});

    dial.addEventListener('touchstart', touchDown);
    dial.addEventListener('touchmove', touchDrag);
    dial.addEventListener('touchend', touchUp);

    var sx = null;
    var sy = null;
    var tempAngle = rotAngle;
    
    knobDict[id] = setValue;
    
    function setValueFromAngle(angle) {
        if (angle < -160) angle = -160;
        if (angle > 160) angle = 160;
        var fraction = (angle + 160) / 320;
        if (power == 0) {
            value = Math.round(min + (max - min) * fraction);
            caption.textContent = value.toFixed(0);
            fraction = (value - min) / (max - min);
            angle = -160 + 320 * fraction;
        }
        else {
            value = min + (max - min) * Math.pow(fraction, power);
            caption.textContent = value.toFixed(2);
        }
        dial.setAttribute("transform", "translate(35, 35) rotate(" + String(angle) + " 0 0) translate(-35, -35)")
        if (valueSetFunction != null)
        {
            valueSetFunction(id, value);
        }
    }
    
    function setValue(newValue)
    {
        value = newValue;
        var fraction = (value - min) / (max - min)
        if (power != 0) fraction = Math.pow(fraction, 1.0/power);
        var rotAngle = -160 + 320 * fraction;
        dial.setAttribute("transform", "translate(35, 35) rotate(" + String(rotAngle) + " 0 0) translate(-35, -35)")
    }
    
    function hover(evt)
    {
        caption.textContent = value.toFixed(power == 0 ? 0 : 2);
    }
    
    function leave(evt)
    {
        if (evt.buttons == 0) caption.textContent = labelText;
    }
    
    function click(evt)
    {
        sx = evt.screenX;
        sy = evt.screenY;
        caption.textContent = value.toFixed(power == 0 ? 0 : 2);
    }
    
    function drag(evt)
    {
        evt.preventDefault();
        if (sy != null)
        {
            var dx = evt.screenX - sx;
            var dy = sy - evt.screenY;
            tempAngle = rotAngle + dy;
            setValueFromAngle(tempAngle);
        }
    }
    
    function release(evt)
    {
        sx = null;
        sy = null;
        caption.textContent = labelText;
        rotAngle = tempAngle;
    }
    
    function touchDown(evt)
    {
        evt.preventDefault();
        sx = evt.touches.item(0).screenX;
        sy = evt.touches.item(0).screenY;
        caption.textContent = value.toFixed(power == 0 ? 0 : 2);
    }
    
    function touchDrag(evt)
    {
        evt.preventDefault();
        if (sy != null)
        {
            var dx = evt.touches.item(0).screenX - sx;
            var dy = sy - evt.touches.item(0).screenY;
            tempAngle = rotAngle + dy;
            setValueFromAngle(tempAngle);
        }
    }
    
    function touchUp(evt)
    {
        evt.preventDefault();
        sx = null;
        sy = null;
        caption.textContent = labelText;
        rotAngle = tempAngle;
    }
};
