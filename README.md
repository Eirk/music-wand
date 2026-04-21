# I Wonder What Happens if I...

![img1](./readme_refs/Neru_Hitting_Miku_In_The_Head_With_A_Leek.gif)

## What Do?
I wanted a music reactive light wand, so I made one. It has different different modes: music and waving. Music mode simply does beat detection and cycles through a color wheel over time as it flashes on beat. Waving mode also cycles through a color wheel over time but it flashes on jerky waving motions so that you can wave it to your own beat :D

# Getting Started

## Building the firmware
First, follow the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) until you can build the hello world example.

If you haven't already, source the zephyr virtual environment so you can use the `west` tool (it's probably at `~/zephyrproject/.venv/bin/activate` if you just blindly followed the official zephyr guide. Don't worry, I'm not judging you)
```
source <path/to/activate> 
```

Initialize a zephyr workspace with
```
west init -m git@github.com:Eirk/music-wand.git <workspace name>
cd <workspace name>
west update
```

Get the required espressif RF binary blobs with 
```
west blobs fetch hal_espressif
```

Now build the application for the target with 
```
west build -b adafruit_feather_esp32s3/esp32s3/procpu --sysbuild music-wand/
```
Flash the application with 
```
west flash
```
Monitor serial output with 
```
west espressif monitor
```
