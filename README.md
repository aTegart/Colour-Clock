# Colour Clock

# Requires

Adafruit Circuit Playground Express (Arduino-compatible)

NeoPixel RGB LED strand

# Summary

The Colour Clock is an Arduino-based project that represents time
as a colour outputed simultaneously on a strand of NeoPixel RGB lights. Rather than staying on all the time,
as that would not be useful, a start and end hour and minute can be set within the program.
Over the window during which it is on, the RGB LEDs cycle once, fading through the rainbow.
It actualy cycles backwards, however, starting and ending on red
(red--violet-->blue--turquoise-->green--yellow-->red).
The start and end on red are because red is the least harsh colour of light on the eyes,
ideal for waking and before sleep (relevent if the colour clock was to be used across a whole day).
The rainbow goes backward so that the majority of the blue light happens earlier in the cycle,
not only to prevent unnecessary blue-light exposure before sleep,
but to gently introduce blue light to encourage wakefulness as the day begins.

The Colour Clock is also useful for tracking time over shorter periods, like one Pomodoro working session. 

# Pins

DATA - 6

Sliding switch (internal) - 7

Pins, number of LEDs, start and end times, and brightness can all be changed at the top of the code to match your personal preferences and hardware needs.
