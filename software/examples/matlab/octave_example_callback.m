function octave_example_callback()
    more off;

    HOST = "localhost";
    PORT = 4223;
    UID = "XYZ"; % Change XYZ to the UID of your Distance IR Bricklet 2.0

    ipcon = javaObject("com.tinkerforge.IPConnection"); % Create IP connection
    dir = javaObject("com.tinkerforge.BrickletDistanceIRV2", UID, ipcon); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register distance callback to function cb_distance
    dir.addDistanceCallback(@cb_distance);

    % Set period for distance callback to 1s (1000ms) without a threshold
    dir.setDistanceCallbackConfiguration(1000, false, "x", 0, 0);

    input("Press key to exit\n", "s");
    ipcon.disconnect();
end

% Callback function for distance callback
function cb_distance(e)
    fprintf("Distance: %g cm\n", e.distance/10.0);
end
