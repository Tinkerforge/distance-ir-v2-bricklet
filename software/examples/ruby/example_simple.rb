#!/usr/bin/env ruby
# -*- ruby encoding: utf-8 -*-

require 'tinkerforge/ip_connection'
require 'tinkerforge/bricklet_distance_ir_v2'

include Tinkerforge

HOST = 'localhost'
PORT = 4223
UID = 'XYZ' # Change XYZ to the UID of your Distance IR Bricklet 2.0

ipcon = IPConnection.new # Create IP connection
dir = BrickletDistanceIRV2.new UID, ipcon # Create device object

ipcon.connect HOST, PORT # Connect to brickd
# Don't use device before ipcon is connected

# Get current distance
distance = dir.get_distance
puts "Distance: #{distance/10.0} cm"

puts 'Press key to exit'
$stdin.gets
ipcon.disconnect
