# Raspberry Pi GPIO Matrix Keyboard
User space GPIO matrix keyboard driver for Raspbery Pi. The code is designed for Commodore Plus/4 keyboard layout, but can be easily modifed to support other 80s and 90s computers' keyboards (Commodore 64, 8-bit Atari, Amiga, etc.)

## Compilation
```
cmake .
make
```
## Installation
To install the keyboard driver as a systemd serice
```
sudo cmake install
```
## Commodore Plus/4 Wiring
This is the wiring used to define the `cols`, `rows`, and `keymap`. Additionally the LED pins can be connected to a free GPIO and GND for extra functionality (or simply +5v).
![Plus/4 Keyboard Matrix](cplus4_keyboard_matrix.png?raw=true "Plus/4 Keyboard Matrix")

| Plus/4 Pin | Pi GPIO Pin | GPIO No. | Matrix map |
|-----------:|------------:|---------:|-----------:|
|           1|            3|         2|    Row 6   |
|           2|            7|         4|    Col 4   |
|           3|            -|         -|  LED GND   |
|           4|            -|         -|    LED     |
|           5|            8|        14|    Row 1   |
|           6|           10|        15|    Col 5   |
|           7|           11|        17|    Row 2   |
|           8|           12|        18|    Col 6   |
|           9|           13|        27|    Col 7   |
|          10|           15|        22|    Row 4   |
|          11|           16|        23|    Row 3   |
|          12|           18|        24|    Row 5   |
|          13|           19|        10|    Col 3   |
|          14|           21|         9|    Col 2   |
|          15|           22|        25|    Row 7   |
|          16|           23|        11|    Col 8   |
|          17|           24|         8|    Col 1   |
|          18|           26|         7|    Row 8   |

## Python Prototype
The Python script `gpio_keyboard.py` is my initial version of this driver. It has basically the same funcitonality except:
* Python `evdev` does not seem to support `EV_REP` on the uinput objects, so the emulated keyboard will not generate auto-repeat
* At 60Hz scanning frequency it takes about 6% CPU compared to <1% for the C version.
