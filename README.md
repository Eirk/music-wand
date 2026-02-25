# Pretty music wand thing
## What it is
This is supposed to be a neat music reactive light wand for concerts/dancing settings

# Getting Started
I haven't selected a target board yet but I'm thinking it'll just be an esp32 built using the zephyr RTOS

## Building the firmware
First, follow the [Zephyr Getting Started Guide](https://docs.zephyrproject.org/latest/develop/getting_started/index.html) until you can build the hello world example.

If you haven't already, source the zephyr virtual environment so you can use the `west` tool (it's probably at `~/zephyrproject/.venv/bin/activate` if you just blindly followed the official zephyr guide. Don't worry, I'm not judging you)
```
source <path/to/activate> 
```

Initialize a zephyr workspace with ( example below uses ssh git clone but you can use https too)
```
west init -m git@github.com:Eirk/music-wand <workspace name>
cd <workspace name>
west update
```

Now build the application for the target with (build with whatever board target you want, it doesn't matter yet)
```
west build -p -b <board_name> music-wand\uc-app
```
Flash the application with 
```
west flash
```