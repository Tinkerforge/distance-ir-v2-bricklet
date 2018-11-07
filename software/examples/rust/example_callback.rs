use std::{error::Error, io, thread};
use tinkerforge::{distance_ir_v2_bricklet::*, ip_connection::IpConnection};

const HOST: &str = "localhost";
const PORT: u16 = 4223;
const UID: &str = "XYZ"; // Change XYZ to the UID of your Distance IR Bricklet 2.0.

fn main() -> Result<(), Box<dyn Error>> {
    let ipcon = IpConnection::new(); // Create IP connection.
    let dir = DistanceIrV2Bricklet::new(UID, &ipcon); // Create device object.

    ipcon.connect((HOST, PORT)).recv()??; // Connect to brickd.
                                          // Don't use device before ipcon is connected.

    // Create receiver for distance events.
    let distance_receiver = dir.get_distance_receiver();

    // Spawn thread to handle received events. This thread ends when the `dir` object
    // is dropped, so there is no need for manual cleanup.
    thread::spawn(move || {
        for distance in distance_receiver {
            println!("Distance: {} cm", distance as f32 / 10.0);
        }
    });

    // Set period for distance callback to 1s (1000ms) without a threshold.
    dir.set_distance_callback_configuration(1000, false, 'x', 0, 0);

    println!("Press enter to exit.");
    let mut _input = String::new();
    io::stdin().read_line(&mut _input)?;
    ipcon.disconnect();
    Ok(())
}
