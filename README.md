# I AM MAD SCIENTIST, SO COOL! SONUVABITCH!
Makise announced surprisng thesis in the Far Eastern island nation.This thesis "Even if it is impossible, each man's making to data is possible it is making each man's "Memory" data."

The possibility is bold and it discusses in detail. The research and study are that credibility is very high, nd it is certain only in possible in a present technology and knowledge will
advance though it has already gotten excited pros and cons about the content of the thesis and the theory of saying [gi] is unfolded in not only the physics in the future but
also the field perhaps the composed theory. Hereafter, the field perhaps the composed theory. Hereftare, the theses gyrus on the temporal lobe, and all man's 


Kurisu=Makise(18)
from U.S. in Graduate school
The thesis is annouced aggressively though it is a graduate school The various circles a new wind is given. though the theory that origin the edge is more than a novel conception by a sharp,
adequate Budding and spirited those who study young man by expectation

![img1](./readme/makise_comedy_cemetery.jpg)

## Theory of Operation
First, a foundational measurement of local gravity is taken and then used as a value of reference, wherein after time traveling, the meter generates a local gravitational sine wave, reproducing the tenets of the Kerr Black Hole theory, then measuring gravity via the onboard gravitational distortion apparatus in order to calculate the rate of variation.

I don't know wtf that means but that's how they explained it in the story. We're just using nixie tubes and cheap hobbyist microcontrollers lol.

# Getting Started
Okay, for real tho here are instructions to get this thing up and running.

## Building the firmware
First, follow the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) until you can build the hello world example.

If you haven't already, source the zephyr virtual environment so you can use the `west` tool (it's probably at `~/zephyrproject/.venv/bin/activate` if you just blindly followed the official zephyr guide. Don't worry, I'm not judging you)
```
source <path/to/activate> 
```

Initialize a zephyr workspace with
```
west init -m git@github.com:Eirk/divergence-meter <workspace name>
cd <workspace name>
west update
```

### If you are bad and bougie and have a jlink: 
Now build the application for the target with 
```
west build -p -b adafruit_feather_nrf52840 divergence-meter\uc-app
```
Flash the application with 
```
west flash
```

### Broke boys without a jlink: 
Oops we got a board without an onboard programmer and have to use uf2. Supa hacka time. Build the application for the target with 
```
cd divergence-meter
west build -p auto -b adafruit_feather_nrf52840/nrf52840/uf2 uc-app
```

Turns out the default zephyr uf2 runner is broken. Copy doesn't work, but copyfile does. Apply this fix by copying the local uf2.py file to zephyr/scripts/west_commands/runners/uf2.py and replace it in the destination. 

Next, ensure the board is in the bootloader. Tap the reset button twice, and the device will show up as FTHR840BOOT and the ws2812 LED should be on at solid red.

Flash the application with 
```
west flash
```
