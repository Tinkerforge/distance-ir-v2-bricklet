function matlab_example_callback()
    import com.tinkerforge.IPConnection;
    import com.tinkerforge.BrickletDistanceIRV2;

    HOST = 'localhost';
    PORT = 4223;
    UID = 'XYZ'; % Change XYZ to the UID of your Distance IR Bricklet 2.0

    ipcon = IPConnection(); % Create IP connection
    dir = handle(BrickletDistanceIRV2(UID, ipcon), 'CallbackProperties'); % Create device object

    ipcon.connect(HOST, PORT); % Connect to brickd
    % Don't use device before ipcon is connected

    % Register distance callback to function cb_distance
    set(dir, 'DistanceCallback', @(h, e) cb_distance(e));

    % Set period for distance callback to 1s (1000ms) without a threshold
    dir.setDistanceCallbackConfiguration(1000, false, 'x', 0, 0);

    input('Press key to exit\n', 's');
    ipcon.disconnect();
end

% Callback function for distance callback
function cb_distance(e)
    fprintf('Distance: %g cm\n', e.distance/10.0);
end
