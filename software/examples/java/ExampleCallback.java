import com.tinkerforge.IPConnection;
import com.tinkerforge.BrickletDistanceIRV2;

public class ExampleCallback {
	private static final String HOST = "localhost";
	private static final int PORT = 4223;

	// Change XYZ to the UID of your Distance IR Bricklet 2.0
	private static final String UID = "XYZ";

	// Note: To make the example code cleaner we do not handle exceptions. Exceptions
	//       you might normally want to catch are described in the documentation
	public static void main(String args[]) throws Exception {
		IPConnection ipcon = new IPConnection(); // Create IP connection
		BrickletDistanceIRV2 dir = new BrickletDistanceIRV2(UID, ipcon); // Create device object

		ipcon.connect(HOST, PORT); // Connect to brickd
		// Don't use device before ipcon is connected

		// Add distance listener
		dir.addDistanceListener(new BrickletDistanceIRV2.DistanceListener() {
			public void distance(int distance) {
				System.out.println("Distance: " + distance/10.0 + " cm");
			}
		});

		// Set period for distance callback to 1s (1000ms) without a threshold
		dir.setDistanceCallbackConfiguration(1000, false, 'x', 0, 0);

		System.out.println("Press key to exit"); System.in.read();
		ipcon.disconnect();
	}
}
