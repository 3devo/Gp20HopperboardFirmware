GP20 Hopperboard firmware
-------------------------
This repository contains the source code for the firmware running on the
hopper board of the 3devo GP20 plastic shredder.

To compile this firmware, the
[(3devo-modified) stm32duino](https://github.com/3devo/Arduino_Core_STM32)
core and [Arduino-3devo](https://github.com/3devo/Arduino-3devo) board
description files need to be installed into the `hardware` subdirectory
of your Arduino sketchbook (these two are also included as submodules in
this repository, so if you configure the Arduino IDE Sketchbook path to
point to this repository, they should also be loaded as needed). Then
select "GP20 Hopperboard" in the IDE and compile the firmware.

A compiled version of this firmware is normally included in the GP20's
main firmware, and uploaded on every startup through
a [bootloader](https://github.com/3devo/ChildbusBootloader) running on the
microcontroller.

License
-------
This firmware is very distantly based on
[bootloader-attiny](https://github.com/modulolabs/bootloader-attiny)
written by Erin Tomson. All code in this repository  is licensed under
the GPL, version 3. See the LICENSE file for the full license.
