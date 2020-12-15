/*
 * general_settings.h
 *
 *  Created on: Nov 2017
 *      Author: M Mata
 */

#ifndef STDIO_H
#define STDIO_H
#include <stdio.h>
#endif

#ifndef GENERAL_SETTINGS_H_
#define GENERAL_SETTINGS_H_

#define LCD_BACKLIGHT_LEVEL 5 //0 to 16
#define LCD_CONTRAST_LEVEL 90 //70 to 127

#define HALF_RACKET_SIZE 16 //racket width in pixels
#define QUARTER_RACKET_SIZE 8
#define BALL_RADIUS 1 //ball radius in pixels

#define PROJECTILE_HALF_SIZE 2 // projectile radius in pixels
#define BONUS_RADIUS 5 // bonus radius in pixels
#define BONUS_APPEAR_X 70
#define BONUS_APPEAR_Y 30


//Timing intervals for the game
#define TIMING_BASE_mS 5 //base timing interval for all timed execution
#define LCD_INTERVAL_mS 40 //timing interval for updating LCD
#define INPUT_INTERVAL_mS 100 //timing interval for reading inputs
#define GAME_INTERVAL_mS 50 //timing interval for updating ball position
//flags marking when updates must be done
volatile unsigned int InputUpdatePending;
volatile unsigned int InterruptTimeoutPending;
volatile unsigned int GameUpdatePending;
volatile unsigned int LCDUpdatePending;
volatile unsigned int ContinuousPressChecker;

//Game state
enum gameState {INTRO,STARTING,MOVING,SCORING,WINNING};
volatile enum gameState gameStateInstance; //Current game state
volatile int newPositionsDrawn;
volatile int newAIPositionsDrawn;

// Banner handling
volatile char bannerString[17];

//Ball handling
volatile int xBall, yBall; //Current ball position
volatile int xBall_old, yBall_old; //For ball trail position
volatile int xBall_old2, yBall_old2; //To delete old ball position
volatile int x_displacement;
volatile int y_displacement;

//Racket handling
enum VDir {STOP, UP, DOWN}; //last racket movement vertical directions
enum LastHitter {P1,P2};
volatile enum LastHitter LastHitterInstance;

//Racket1's variables
volatile int xR1, yR1, xR2, yR2; //Current racket 1 and racket 2 positions
volatile int xR1_old, yR1_old, xR2_old, yR2_old; //To delete old racket position
volatile int yR1_previousPosition, yR2_previousPosition; // For if yR1_old gets overwritten with yR1 by racket staying still, but there is still a previous position
// NB yR1_old will also be used to check if racket position is different from old Position, and thereby determining if it is moving up or down, or not moving
volatile enum VDir R1Dir; //last movement direction for racket 1 (used for ball control)
volatile enum VDir R2Dir; //last movement direction for racket 2 (used for ball control)
volatile enum VDir R2bonusDir; // last movement direction for player 2 bonus racket

//Score handling
enum PlayerEnum {PLAYER1,PLAYER2};
volatile int p1Score,p2Score;
volatile char scoreString[14];
volatile char currentScoresString[15];
volatile char winningString[13];
volatile int winningScore;


volatile enum PlayerEnum Scorer;
volatile enum PlayerEnum Player;

//AI handling
volatile int AI_enabled;
volatile int toggle_AI_direction;

//Bonus handling
enum BonusFSM {INPLAY,PICKEDUP,OUTOFPLAY};
volatile enum BonusFSM BonusStatus;
volatile int bonusScore; // the score at which the bonus will appear
volatile int bonusDrawn; // for wether to redraw the bonus
volatile int bonusX, bonusY; // Position of the bonus on the screen
// player1 projectile handling
volatile int p1bonusEnabled; // bonus enabled or not for this player
volatile int p1bonusCooldown;
volatile int p1Projectiles_onscreen;
volatile int p1Projectiles_active;
volatile int p1Projectiles_pressed;
volatile int p1ProjectileA_X, p1ProjectileA_Y,p1ProjectileB_X, p1ProjectileB_Y; //Current projectile positions
volatile int p1ProjectileA_X_old, p1ProjectileA_Y_old,p1ProjectileB_X_old, p1ProjectileB_Y_old; //For projectile previous positions
volatile int p1ProjectileA_X_old2, p1ProjectileA_Y_old2,p1ProjectileB_X_old2, p1ProjectileB_Y_old2; //To delete old projectile positions
volatile int p1_projectileA_x_displacement,p1_projectileB_x_displacement; // to set x_displacement of projectiles
//player2 projectile handling
volatile int p2bonusEnabled; // bonus enabled or not for this player
volatile int p2bonusCooldown;
volatile int xR2bonus, yR2bonus; //Current player2 bonus racket positions
volatile int xR2bonus_old, yR2bonus_old; //To delete old racket position
volatile int yR2bonus_previousPosition; // For if yR1_old gets overwritten with yR1 by racket staying still, but there is still a previous position


#endif /* GENERAL_SETTINGS_H_ */
