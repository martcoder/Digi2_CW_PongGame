# PongGame

This is based on the code providing by Mario Mata. 

The original code provided the functionality to play pong on an LCD attached
to and MSP430 microcontroller via the experimenter board. 

Features from original code are the setup and hardware implementation for 
the LCD to show a single racket on the left of the screen which moves upward
when a switch on the experimenter board is pressed; plus a ball 
moving sideways on initialisation. 

Gameplay Features added to this include: 
- RACKET SIZE: Increasing racket size, which necesitated optimising how it is drawn too
- JOYSTICK: Using joystick for UP/DOWN movements of racket
- 2PLAYER: 2 player game, using BUTTONSs to control UP/DOWN for second player
- VS AI: 1 player vs AI game
- INTRO MENU: Intro menu to choose game mode, 2 player OR 1 player vs AI. 
- RACKET MOVEMENT EFFECT: Racket movement affects ball e.g. moving racket up when ball strikes will
  result in the ball gaining some upward direction, and vice versa. 
- RACKET ZONES: Racket have directional zones, e.g. when ball strikes a non-moving racket
  it will change direction depending on where it hits the racket, centre 
  zone will keep current up/down movement; upper zone will provide the ball
  with upward directional movement; lower zone will provide downward direction. 
- SCORING TIMEOUT: When a player scores, the game is paused, scores are updated, a 'Goal Scored' 
  message shows and the rackets are slid back to their starting positions. 
  Once rackets reach their starting positions the game continues with the ball 
  moving toward the racket of the player who just had a goal scored against them. 
- BONUSES: Bonus ball appears when either player reaches score of 1. 
  If player1 hits the bonus with the ball, they get twin cannons added their racket which can fire 
  quantum-linked energy blasts when JOYSTICK-RIGHT is pressed. 
  If player2 hits the bonus with the ball they get an bonus extra mini racket which moves in the opposite 
  direction to their normal racket. The bonus racket is half the size and still offers a racket 
  movement effect but it lacks racket zones. 
  Bonuses have a cooldown, which is decremented each time the opponent scores, until the bonus is 
  deactivated for the player whos cooldown reaches zero. 
- BANNER: A banner across the top of the LCD displays up-to-date scores and whether
  players have a bonus activated currently. 
- SPLASH SCREENS: Players can continue after splash screens by pressing an input, which 
  triggers the input interrupt ISR which ensures things carry on smoothly.
  E.g. the intro menu screen, or the GAME OVER screen. 
- END OF GAME: Added End of Game. first player to reach the winning score wins the game. 
  Then by pressing an input or the Reset btn, the game will start again. 


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
