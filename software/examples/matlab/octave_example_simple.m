function octave_example_simple()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Distance IR Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    dir = javaObject("com.tinkerforge.BrickletDistanceIRV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Get current distance
    distance = dir.getDistance();
    fprintf("Distance: %g cm\n", distance/10.0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end
