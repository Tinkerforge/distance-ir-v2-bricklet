package main

import (
	"fmt"
	"tinkerforge/distance_ir_v2_bricklet"
	"tinkerforge/ipconnection"
)

const ADDR string = "localhost:4223"
const UID string = "XYZ" // Change XYZ to the UID of your Distance IR Bricklet 2.0.

func main() {
	ipcon := ipconnection.New()
	defer ipcon.Close()
	dir, _ := distance_ir_v2_bricklet.New(UID, &ipcon) // Create device object.

	ipcon.Connect(ADDR) // Connect to brickd.
	defer ipcon.Disconnect()
	// Don't use device before ipcon is connected.

	dir.RegisterDistanceCallback(func(distance uint16) {
		fmt.Printf("Distance: %f cm\n", float64(distance)/10.0)
	})

	// Configure threshold for distance "smaller than 30 cm"
	// with a debounce period of 1s (1000ms).
	dir.SetDistanceCallbackConfiguration(1000, false, '<', 30*10, 0)

	fmt.Print("Press enter to exit.")
	fmt.Scanln()

}
