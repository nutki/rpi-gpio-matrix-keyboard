#!/usr/bin/python3
import RPi.GPIO as GPIO
from time import sleep
from evdev import UInput, ecodes as e

ui = UInput(name = "Commodore Pi/4 Keyboard", vendor = 0x1209, product = 0x7501)
cols = [8, 9, 10, 4, 15, 18, 27, 11]
rows = [14, 17, 23, 22, 24, 2, 25, 7]
keymap = [
    e.KEY_1,         e.KEY_DELETE,    e.KEY_LEFTCTRL,   e.KEY_LEFTALT,   e.KEY_SPACE, e.KEY_LEFTMETA,  e.KEY_Q,     e.KEY_2,
    e.KEY_3,         e.KEY_W,         e.KEY_A,          e.KEY_LEFTSHIFT, e.KEY_Z,     e.KEY_S,         e.KEY_E,     e.KEY_4,
    e.KEY_5,         e.KEY_R,         e.KEY_D,          e.KEY_X,         e.KEY_C,     e.KEY_F,         e.KEY_T,     e.KEY_6,
    e.KEY_7,         e.KEY_Y,         e.KEY_G,          e.KEY_V,         e.KEY_B,     e.KEY_H,         e.KEY_U,     e.KEY_8,
    e.KEY_9,         e.KEY_I,         e.KEY_J,          e.KEY_N,         e.KEY_M,     e.KEY_K,         e.KEY_O,     e.KEY_0,
    e.KEY_DOWN,      e.KEY_P,         e.KEY_L,          e.KEY_COMMA,     e.KEY_DOT,   e.KEY_SEMICOLON, e.KEY_MINUS, e.KEY_UP,
    e.KEY_LEFT,      e.KEY_BACKSLASH, e.KEY_APOSTROPHE, e.KEY_SLASH,     e.KEY_ESC,   e.KEY_EQUAL,     e.KEY_GRAVE, e.KEY_RIGHT,
    e.KEY_BACKSPACE, e.KEY_ENTER,     e.KEY_RIGHTBRACE, e.KEY_LEFTBRACE, e.KEY_F1,    e.KEY_F2,        e.KEY_F3,    e.KEY_F4,
]

GPIO.setmode(GPIO.BCM)
GPIO.setup(rows, GPIO.IN)
GPIO.setup(cols, GPIO.IN, pull_up_down = GPIO.PUD_UP)
pressed = set()
while True:
    sleep(1/60)
    syn = False
    for i in range(len(rows)):
        GPIO.setup(rows[i], GPIO.OUT, initial = GPIO.LOW)
        for j in range(len(cols)):
            keycode = i * len(cols) + j
            newval = GPIO.input(cols[j]) == GPIO.LOW
            if  newval and not keycode in pressed:
                pressed.add(keycode)
                GPIO.output(3, GPIO.HIGH)
                ui.write(e.EV_KEY, keymap[keycode], 1)
                syn = True
            elif not newval and keycode in pressed:
                pressed.discard(keycode)
                GPIO.output(3, GPIO.LOW)
                ui.write(e.EV_KEY, keymap[keycode], 0)
                syn = True
        GPIO.setup(rows[i], GPIO.IN)
    if syn:
        ui.syn()
