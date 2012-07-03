.pragma library

function compareArray(a, b) {
    if (a.length !== b.length)
        return false;

    for (var i = 0; i < a.length; ++i) {
        var aMatched = false;
        var bMatched = false;

        for (var j = 0; j < b.length; ++j) {
            if (a[i] === b[j])
                aMatched = true;
            if (b[i] === a[j])
                bMatched = true;
            if (aMatched && bMatched)
                break;
        }

        if (!aMatched || !bMatched)
            return false;
    }

    return true;
}

function testObjectProperties(testCase, testObject, data) {
    var signalSpy = Qt.createQmlObject('import QtTest 1.0; SignalSpy {}', testCase, "SignalSpy");
    signalSpy.target = testObject;
    signalSpy.signalName = data.signal;

    // set property to something new
    testObject[data.property] = data.value;
    if (data.array) {
        if (data.expectedValue) {
            testCase.verify(compareArray(testObject[data.property], data.expectedValue));
            testCase.compare(signalSpy.count, 1 + data.expectedValue.length);
        } else {
            testCase.verify(compareArray(testObject[data.property], data.value));
            testCase.compare(signalSpy.count, 1 + data.value.length);
        }

    } else {
        testCase.compare(testObject[data.property], data.value);
        testCase.compare(signalSpy.count, 1);
    }

    signalSpy.clear();

    // set property to same value
    testObject[data.property] = data.value;
    if (data.array) {
        if (data.expectedValue) {
            testCase.verify(compareArray(testObject[data.property], data.expectedValue));
            testCase.compare(signalSpy.count, 1 + data.expectedValue.length);
        } else {
            testCase.verify(compareArray(testObject[data.property], data.value));
            testCase.compare(signalSpy.count, 1 + data.value.length);
        }

    } else {
        testCase.compare(testObject[data.property], data.value);
        testCase.compare(signalSpy.count, 0);
    }

    signalSpy.clear();

    // reset property
    if (data.reset === undefined) {
        testObject[data.property] = null;
        testCase.compare(testObject[data.property], null);
    } else {
        testObject[data.property] = data.reset;
        if (data.array)
            testCase.verify(compareArray(testObject[data.property], data.reset));
        else
            testCase.compare(testObject[data.property], data.reset);
    }
    testCase.compare(signalSpy.count, 1);

    signalSpy.destroy();
}
