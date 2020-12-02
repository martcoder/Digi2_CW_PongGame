/*
 * general_settings.h
 *
 *  Created on: Nov 2017
 *      Author: M Mata
 */

#ifndef GENERAL_SETTINGS_H_
#define GENERAL_SETTINGS_H_

#define LCD_BACKLIGHT_LEVEL 5 //0 to 16
#define LCD_CONTRAST_LEVEL 90 //70 to 127

#define HALF_RACKET_SIZE 16 //racket width in pixels
#define BALL_RADIUS 1 //ball radius in pixels

//Timing intervals for the game
#define TIMING_BASE_mS 5 //base timing interval for all timed execution
#define LCD_INTERVAL_mS 40 //timing interval for updating LCD
#define INPUT_INTERVAL_mS 100 //timing interval for reading inputs
#define BALL_INTERVAL_mS 50 //timing interval for updating ball position
//flags marking when updates must be done
volatile unsigned int InputUpdatePending;
volatile unsigned int BallUpdatePending;
volatile unsigned int LCDUpdatePending;

//Game state
enum gameState {BEGINNING,INPLAY,SCORED,END};
volatile enum gameState gameStateInstance;

//Ball handling
enum ballState {STARTING,MOVING,SCORING};
volatile enum ballState ballStateInstance; //Current ball state
volatile int xBall, yBall; //Current ball position
volatile int xBall_old, yBall_old; //For ball trail position
volatile int xBall_old2, yBall_old2; //To delete old ball position
volatile int x_displacement;
volatile int y_displacement;

//Racket handling
enum VDir {STOP, UP, DOWN}; //last racket movement vertical directions
//Racket1's variables
volatile int xR1, yR1; //Current racket 1 position
volatile int xR1_old, yR1_old; //To delete old racket position
// NB yR1_old will also be used to check if racket position is different from old Position, and thereby determining if it is moving up or down, or not moving
volatile enum VDir R1Dir; //last movement direction for racket 1 (used for ball control)

#endif /* GENERAL_SETTINGS_H_ */
