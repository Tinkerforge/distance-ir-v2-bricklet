var Tinkerforge = require('tinkerforge');

var HOST = 'localhost';
var PORT = 4223;
var UID = 'XYZ'; // Change XYZ to the UID of your Distance IR Bricklet 2.0

var ipcon = new Tinkerforge.IPConnection(); // Create IP connection
var dir = new Tinkerforge.BrickletDistanceIRV2(UID, ipcon); // Create device object

ipcon.connect(HOST, PORT,
    function (error) {
        console.log('Error: ' + error);
    }
); // Connect to brickd
// Don't use device before ipcon is connected

ipcon.on(Tinkerforge.IPConnection.CALLBACK_CONNECTED,
    function (connectReason) {
        // Configure threshold for distance "smaller than 30 cm"
        // with a debounce period of 1s (1000ms)
        dir.setDistanceCallbackConfiguration(1000, false, '<', 30*10, 0);
    }
);

// Register distance callback
dir.on(Tinkerforge.BrickletDistanceIRV2.CALLBACK_DISTANCE,
    // Callback function for distance callback
    function (distance) {
        console.log('Distance: ' + distance/10.0 + ' cm');
    }
);

console.log('Press key to exit');
process.stdin.on('data',
    function (data) {
        ipcon.disconnect();
        process.exit(0);
    }
);
