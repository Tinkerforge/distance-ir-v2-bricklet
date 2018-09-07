<?php

require_once('Tinkerforge/IPConnection.php');
require_once('Tinkerforge/BrickletDistanceIRV2.php');

use Tinkerforge\IPConnection;
use Tinkerforge\BrickletDistanceIRV2;

const HOST = 'localhost';
const PORT = 4223;
const UID = 'XYZ'; // Change XYZ to the UID of your Distance IR Bricklet 2.0

// Callback function for distance callback
function cb_distance($distance)
{
    echo "Distance: " . $distance/10.0 . " cm\n";
}

$ipcon = new IPConnection(); // Create IP connection
$dir = new BrickletDistanceIRV2(UID, $ipcon); // Create device object

$ipcon->connect(HOST, PORT); // Connect to brickd
// Don't use device before ipcon is connected

// Register distance callback to function cb_distance
$dir->registerCallback(BrickletDistanceIRV2::CALLBACK_DISTANCE, 'cb_distance');

// Set period for distance callback to 1s (1000ms) without a threshold
$dir->setDistanceCallbackConfiguration(1000, FALSE, 'x', 0, 0);

echo "Press ctrl+c to exit\n";
$ipcon->dispatchCallbacks(-1); // Dispatch callbacks forever

?>
