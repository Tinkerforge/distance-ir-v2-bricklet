use std::{error::Error, io, thread};
use tinkerforge::{distance_ir_v2_bricklet::*, ipconnection::IpConnection};

const HOST: &str = "127.0.0.1";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Distance IR Bricklet 2.0

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection
    let distance_ir_v2_bricklet = DistanceIRV2Bricklet::new(UID, &ipcon); // Create device object

    ipcon.connect(HOST, PORT).recv()??; // Connect to brickd
                                        // Don't use device before ipcon is connected

    //Create listener for distance events.
    let distance_listener = distance_ir_v2_bricklet.get_distance_receiver();
    // Spawn thread to handle received events. This thread ends when the distance_ir_v2_bricklet
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for event in distance_listener {
            println!("Distance: {}{}", event as f32 / 10.0, " cm");
        }
    });

    // Configure threshold for distance "smaller than 30 cm"
    // with a debounce period of 1s (1000ms)
    distance_ir_v2_bricklet.set_distance_callback_configuration(1000, false, '<', 30 * 10, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
