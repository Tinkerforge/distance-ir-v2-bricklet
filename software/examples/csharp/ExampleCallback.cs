using System;
using Tinkerforge;

class Example
{
	private static string HOST = "localhost";
	private static int PORT = 4223;
	private static string UID = "XYZ"; // Change XYZ to the UID of your Distance IR Bricklet 2.0

	// Callback function for distance callback
	static void DistanceCB(BrickletDistanceIRV2 sender, int distance)
	{
		Console.WriteLine("Distance: " + distance/10.0 + " cm");
	}

	static void Main()
	{
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletDistanceIRV2 dir = new BrickletDistanceIRV2(UID, ipcon); // Create device object

		ipcon.Connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Register distance callback to function DistanceCB
		dir.DistanceCallback += DistanceCB;

		// Set period for distance callback to 1s (1000ms) without a threshold
		dir.SetDistanceCallbackConfiguration(1000, false, 'x', 0, 0);

		Console.WriteLine("Press enter to exit");
		Console.ReadLine();
		ipcon.Disconnect();
	}
}
