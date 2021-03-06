#!/usr/bin/perl

use strict;
use Tinkerforge::IPConnection;
use Tinkerforge::BrickletDistanceIRV2;

use constant HOST => 'localhost';
use constant PORT => 4223;
use constant UID => 'XYZ'; # Change XYZ to the UID of your Distance IR Bricklet 2.0

# Callback subroutine for distance callback
sub cb_distance
{
    my ($distance) = @_;

    print "Distance: " . $distance/10.0 . " cm\n";
}

my $ipcon = Tinkerforge::IPConnection->new(); # Create IP connection
my $dir = Tinkerforge::BrickletDistanceIRV2->new(&UID, $ipcon); # Create device object

$ipcon->connect(&HOST, &PORT); # Connect to brickd
# Don't use device before ipcon is connected

# Register distance callback to subroutine cb_distance
$dir->register_callback($dir->CALLBACK_DISTANCE, 'cb_distance');

# Configure threshold for distance "smaller than 30 cm"
# with a debounce period of 1s (1000ms)
$dir->set_distance_callback_configuration(1000, 0, '<', 30*10, 0);

print "Press key to exit\n";
<STDIN>;
$ipcon->disconnect();
