# Litter Robot 3 homebrew controller

Spin the feces for the good of the kitties

<!-- toc -->

- [Warranties and support](#warranties-and-support)
- [Problem statement](#problem-statement)
- [Usage](#usage)
- [Items used](#items-used)
- [Hardware layouts](#hardware-layouts)
- [Status](#status)
- [Licensing](#licensing)

<!-- tocstop -->

## Warranties and support

Absolutely no expressed or implied warranties are attached to your using this codebase.

Issue and pull request submissions are welcome.

## Problem statement

My wife and I dropped over 500$ on a [Litter Robot 3](https://www.litter-robot.com/litter-robot-3.html) nearly three years ago. For some reason we got the extra one year of warranty - without thinking that there's no way we'd be willing to pay for its return shipping.

At first it was fine, but the logic has been in decline over the last year or so. Issues we've run into include but aren't limited to:

- Blue light blinks: means the tray is full (but it's not). Cleaning the sensors may or may not help. Turning on and off may or may not help.
- Yellow light blinks: something else is broken
- Yellow light blinks very fast: anti-pinch logic is broken

At some point, we realized it fails to cycle in the dark, when it turns on the blue illumination. I suspected a power issue but didn't have the time to look into it. Instead, I finalized this project.

In the final assembly, it was apparent that the original Litter Robot power supply (15V 1A) may be at fault: it was unable to drive the motor when connected directly, while a random other one (12V 1A) had no issues in that arena.

## Usage

1. Read [`Makefile`](Makefile) and make sure you know what you're running in these steps
1. `make deps` installs `argparse` and `wiringPi` libraries
1. `make rpi-config` configures the Raspberry Pi
1. `make` builds the binary
1. `make install` installs the binary in `$PATH`
1. `make service` installs and starts the `systemd` service with default values
1. `make logs` shows the logs

Or, `make deploy` covers everything necessary to update after the initial steps have been completed.

## Items used

Item | Comments | Cost
-|-|-
[Basic GPIO kit](https://www.amazon.com/gp/product/B01MATM4XF) | Had all sensors and guidance necessary to start the project off (currently unavailable, but anything similar will work) | 21$
[Relay module](https://www.amazon.com/gp/product/B00E0NTPP4) | Necessary for controlling the motor | 7$
[Raspberry Pi 3B+](https://www.amazon.com/gp/product/B07BDR5PDW) | Most likely any RPi will do, 3B+ is the one I use | 40$
[MicroSD card](https://www.amazon.com/gp/product/B0749KG1JK) | For the Pi | 9$
[Dupont pin crimper](https://www.amazon.com/gp/product/B00OMM4YUY) | Necessary for fabricating cables for sensors | 23$
[Dupont connector kit](https://www.amazon.com/gp/product/B01G0I0ZZK) | Parts for making those sensor cables | 14$
[PCB](https://www.amazon.com/gp/product/B07BF8Z3HS) | Very nice boards, I managed to only need one for this project! | 15$
[Jumper wires](https://www.amazon.com/gp/product/B07CJYSL2T) | Necessary for connecting sensors on the final board | 11$
[Soldering iron](https://www.amazon.com/gp/product/B003H6NN2Q) | My favourite iron | 80$
[Tweezers](https://www.amazon.com/gp/product/B00FZPEWI6) | Sometimes helpful in soldering | 7$
[Solder sucker](https://www.amazon.com/gp/product/B002MJMXD4) | The only solder sucker that ever worked for me | 24$
[Wire cutters](https://www.amazon.com/gp/product/B07GR7QF63) | | 7$
[Solder](https://www.amazon.com/gp/product/B072K22JRT) |  | 9$
[Flux](https://www.amazon.com/gp/product/B008ZIV85A) | It might just be a russian thing to solder with a jar of flux | 9$
[Feather wand](https://www.amazon.com/gp/product/B00LZUCIL8) | The least you can do for taking your cats' toilet away for weeks is play with them | 10$
Total | | 286$

## Hardware layouts

TODO

## Status

This code is relatively stable, the hardware is finished, installed and is in use at home. So, basically, I run this in production.

## Licensing

 * [Apache License, Version 2.0](https://www.apache.org/licenses/LICENSE-2.0): [`./LICENSE-APACHE`](LICENSE-APACHE)
 * [MIT License](https://opensource.org/licenses/MIT): [`./LICENSE-MIT`](LICENSE-MIT)

Licensed at your option of either of the above licenses.
