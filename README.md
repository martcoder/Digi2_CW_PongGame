# Digi2_CW_PongGame

This is based on the code providing by Mario Mata. 

The original code provided the functionality to play pong on an LCD attached
to and MSP430 microcontroller via the experimenter board. 

Features from original code are the setup and hardware implementation for 
the LCD to show a single racket on the left of the screen which moves upward
when a switch on the experimenter board is pressed; plus a ball 
moving sideways on initialisation. 

Gameplay Features added to this include: 
- Increasing racket size, which necesitated optimisation how it is drawin too
- Using joystick for UP/DOWN movements of racket
- Allowing the current movement of the racket to affect ball bounce off
- 2 player game, using BUTTONSs to control UP/DOWN for second player
- Goal splash screen showing which player scored and current scores totals
- Players can continue after the splash screen by pressing an input, which 
  triggers the input interrupt ISR which ensures things carry on smoothly. 


Other Features: 
- The MSP430 microcontroller has a lot of possibilities for power saving, and
  low power modes were already made use of, with Timers triggering interrupts 
  where the ISR would set a var then wake the CPU, which would continue it's 
  inifinite loop and do actions based on which vars had been set, e.g. a var 
  to update positions, a var for updating ball position. The original code 
  had a timer for ball and lcd update timings, and another for user input. 
- The second timer was scrapped and user inputs were configured to trigger 
  interrupts instead, with the ISR setting the variable. 
  The Timer which was being used for ball and lcd updates does this by waking 
  the CPU, so this has been used as an opportunity to also carry out other CPU 
  tasks such as updating positions and actions based on user input. The upshot 
  is that there is one less timer being used, which saves power. 
