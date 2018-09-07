Imports System
Imports Tinkerforge

Module ExampleCallback
    Const HOST As String = "localhost"
    Const PORT As Integer = 4223
    Const UID As String = "XYZ" ' Change XYZ to the UID of your Distance IR Bricklet 2.0

    ' Callback subroutine for distance callback
    Sub DistanceCB(ByVal sender As BrickletDistanceIRV2, ByVal distance As Integer)
        Console.WriteLine("Distance: " + (distance/10.0).ToString() + " cm")
    End Sub

    Sub Main()
        Dim ipcon As New IPConnection() ' Create IP connection
        Dim dir As New BrickletDistanceIRV2(UID, ipcon) ' Create device object

        ipcon.Connect(HOST, PORT) ' Connect to brickd
        ' Don't use device before ipcon is connected

        ' Register distance callback to subroutine DistanceCB
        AddHandler dir.DistanceCallback, AddressOf DistanceCB

        ' Set period for distance callback to 1s (1000ms) without a threshold
        dir.SetDistanceCallbackConfiguration(1000, False, "x"C, 0, 0)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
