# Digi2_CW_PongGame

This is based on the code providing by Mario Mata. 

The original code provided the functionality to play pong on an LCD attached
to and MSP430 microcontroller via the experimenter board. 

Features from original code are the setup and hardware implementation for 
the LCD to show a single racket on the left of the screen which moves upward
when a switch on the experimenter board is pressed; plus a ball 
moving sideways on initialisation. 

Features added to this include: 
- Increasing racket size, which necesitated optimisation its drawing too for
- Using joystick for UP/DOWN movements of racket
- Allowing the current movement of the racket to affect ball bounce off

