#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Distance IR Bricklet 2.0

# Handle incoming distance callbacks
tinkerforge dispatch distance-ir-v2-bricklet $uid distance &

# Set period for distance callback to 1s (1000ms) without a threshold
tinkerforge call distance-ir-v2-bricklet $uid set-distance-callback-configuration 1000 false threshold-option-off 0 0

echo "Press key to exit"; read dummy

kill -- -$$ # Stop callback dispatch in background
