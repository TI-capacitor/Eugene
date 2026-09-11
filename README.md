# Eugene v0.2.0 — Pre-alpha

Bluetooth-controlled rover built on an Arduino Uno and an L298N dual H-bridge. Commands are sent over a ZS-040 Bluetooth module from *Serial Bluetooth Terminal* and parsed into movement calls. The main loop is non-blocking (no `delay()`), with a state machine reserved for the upcoming scanning and autonomous modes.

## Installation

On linux:

``` bash
git clone git@github.com:TI-capacitor/Eugene.git
```

Then change into the directory:

``` bash
cd Eugene
```

Open `src/car.ino` in the Arduino IDE, select **Arduino Uno** as the board, and upload. Pair with the ZS-040 module and connect at **9600 baud**.

## Structure

The files are ordered as follows:

* `src/` <-- code
* `docs/` <-- wiring diagrams and presentation

## Pin assignment

| Signal | Pin | Role |
| --- | --- | --- |
| `In1` | 7 | Left motor direction A |
| `In2` | 8 | Left motor direction B |
| `In3` | 9 | Right motor direction A |
| `In4` | 10 | Right motor direction B |
| `EnA` | 6 | Left motor PWM (must be a PWM pin) |
| `EnB` | 11 | Right motor PWM (must be a PWM pin) |

## Overview of Eugene.h

### Command class

``` c++
class Command
{
  public:
    void setCommand();
    void parseCommand();
    void listOfCommands();
};
```

* `setCommand()`: Reads available bytes from the serial buffer one character at a time and appends them to the internal string until a newline arrives, which flags the command as complete. Non-blocking — it returns immediately when nothing is available.
* `parseCommand()`: Trims and lowercases the buffered string, matches it against the known commands, and clears the buffer afterwards. Unrecognized input falls through to `listOfCommands()`.
* `listOfCommands()`: Prints the command menu to the serial terminal.

### Movement class

``` c++
class Movement
{
  public:
    Movement(int,int,int,int,int,int);
    void begin();
    void stopMotors();
    void forward();
    void backward();
    void pivotRight();
    void pivotLeft();
};
``` 

* `Movement(int,int,int,int,int,int)`: Constructor. Takes the six pin numbers in the order `In1, In2, In3, In4, EnA, EnB`.
* `begin()`: Sets all six pins to `OUTPUT`. Called once from `setup()`.
* `stopMotors()`: Drives all four direction pins LOW and sets both PWM channels to 0.
* `forward()`: Both wheels driven in the forward direction at full PWM.
* `backward()`: Both wheels driven in reverse at full PWM.
* `pivotRight()`: Wheels driven in opposite directions to rotate the rover in place clockwise.
* `pivotLeft()`: Wheels driven in opposite directions to rotate the rover in place counter-clockwise.

## Command reference

| Command | Action |
| --- | --- |
| `front` | Moves the rover forward |
| `back` | Moves the rover backward |
| `stop` | Halts movement |
| `pivot.left` | Pivots the rover to the left |
| `pivot.right` | Pivots the rover to the right |

Commands are case-insensitive and whitespace is trimmed.

## State machine

`car.ino` defines three rover states:

``` c++
enum ROVER_STATE{BT_CONTROL=1,SCAN,AUTONOMOUS};
```

* `BT_CONTROL` — default state. Commands come in over Bluetooth and drive the motors directly.
* `SCAN` — reserved for ultrasound sweeping. Not implemented in v0.2.0.
* `AUTONOMOUS` — reserved for the autonomy framework. Not implemented in v0.2.0.

State work is gated behind a `millis()` comparison with a 100 ms interval, so the loop never blocks and serial input keeps being serviced every pass.

## Changelog v0.2.0

* Elimination of `delay()` in `loop()`
* Elimination of LCD and ultrasound code to focus on movement development
* Addition of Bluetooth module ZS-040 to control the rover via *Serial Bluetooth Terminal*
* Addition of classes for abstraction and a cleaner main script
* Better power management using 3.7 V 18650 batteries in 2S configuration OUT NOW!!!

## Video

* The current version will be documented on my YouTube channel
* Video will show main features and the troubleshooting process
* The video is set to release in autumn of 2026

Demo video (v0.1.0): https://www.youtube.com/watch?v=5wAjBlTuVfc

## Hardware components used

Links are the exact items used in Eugene v0.1.0 (FYI: not sponsored).

* Arduino Uno:
https://www.amazon.com/Arduino-A000066-ARDUINO-UNO-R3/dp/B008GRTSV6/
* Geared motors (TT DC motors, 3–6 V class):
https://www.amazon.com/AEDIKO-Motor-Gearbox-200RPM-Ratio/dp/B09N6NXP4H
* Motor driver (L298N dual H-bridge module):
https://www.amazon.com/BOJACK-H-Bridge-Controller-Intelligent-Mega2560/dp/B0C5JCF5RS
* Batteries (18650 cells):
https://www.amazon.com/AFSONGOO-Rechargeable-Flashlights-Headlamps-Doorbells/dp/B0CSSZXLLC/
* Chassis CAD (by Ian Carey, Thingiverse):
https://www.thingiverse.com/thing:5556192
* Castor wheel (small swivel caster):
https://www.amazon.com/Adhesive-Rotation-Universal-Storage-Furniture/dp/B0B4SGYDFH
* Switches (small DC slide switches):
https://www.amazon.com/Tnuocke-Vertical-Position-Latching-SS12F15-G5/dp/B099N3HFPG/

## Roadmap

* Wire `parseCommand()` to the `Movement` methods so commands actually drive the motors
* Reintroduce the ultrasound sensor under the `SCAN` state
* Autonomy framework under the `AUTONOMOUS` state
