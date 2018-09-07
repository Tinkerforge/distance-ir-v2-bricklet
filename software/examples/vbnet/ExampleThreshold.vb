Imports System
Imports Tinkerforge

Module ExampleThreshold
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

        ' Configure threshold for distance "smaller than 30 cm"
        ' with a debounce period of 1s (1000ms)
        dir.SetDistanceCallbackConfiguration(1000, False, "<"C, 30*10, 0)

        Console.WriteLine("Press key to exit")
        Console.ReadLine()
        ipcon.Disconnect()
    End Sub
End Module
