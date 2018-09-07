#!/bin/sh
# Connects to localhost:4223 by default, use --host and --port to change this

uid=XYZ # Change XYZ to the UID of your Distance IR Bricklet 2.0

# Get current distance
tinkerforge call distance-ir-v2-bricklet $uid get-distance
