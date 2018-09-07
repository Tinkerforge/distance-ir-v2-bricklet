#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Distance IR Bricklet 2.0

# Handle incoming distance callbacks
tinkerforge dispatch distance-ir-v2-bricklet $uid distance &

# Configure threshold for distance "smaller than 30 cm"
# with a debounce period of 1s (1000ms)
tinkerforge call distance-ir-v2-bricklet $uid set-distance-callback-configuration 1000 false threshold-option-smaller 300 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
