program ExampleSimple;

{$ifdef MSWINDOWS}{$apptype CONSOLE}{$endif}
{$ifdef FPC}{$mode OBJFPC}{$H+}{$endif}

uses
  SysUtils, IPConnection, BrickletDistanceIRV2;

type
  TExample = class
  private
    ipcon: TIPConnection;
    dir: TBrickletDistanceIRV2;
  public
    procedure Execute;
  end;

const
  HOST = 'localhost';
  PORT = 4223;
  UID = 'XYZ'; { Change XYZ to the UID of your Distance IR Bricklet 2.0 }

var
  e: TExample;

procedure TExample.Execute;
var distance: word;
begin
  { Create IP connection }
  ipcon := TIPConnection.Create;

  { Create device object }
  dir := TBrickletDistanceIRV2.Create(UID, ipcon);

  { Connect to brickd }
  ipcon.Connect(HOST, PORT);
  { Don't use device before ipcon is connected }

  { Get current distance }
  distance := dir.GetDistance;
  WriteLn(Format('Distance: %f cm', [distance/10.0]));

  WriteLn('Press key to exit');
  ReadLn;
  ipcon.Destroy; { Calls ipcon.Disconnect internally }
end;

begin
  e := TExample.Create;
  e.Execute;
  e.Destroy;
end.
