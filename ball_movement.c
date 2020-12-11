/*********************************************************
 * ball_movement.c
 *
 * Implements the state diagram for the ball's movement
 * Only a few states are completed, refer to the CW description
 * to find out about the remaining states
 *
 * Created on: Nov 2017
 * Author: M MATA (GCU)
 *********************************************************/
#include "ball_movement.h"
#include "general_settings.h"
#include "hal_lcd.h"
#include "msp430f5438a.h"
#include "lookup.c"



int top_wall_reached()
{
    if(yBall <= (BALL_RADIUS) ) //top wall reached
    {
        yBall = 1+BALL_RADIUS; //do not overwrite the wall
        return 1;
    }
    else return 0;
}

int bottom_wall_reached()
{
    if(yBall >= (LCD_ROW-BALL_RADIUS)) //up wall reached
    {
        yBall = LCD_ROW-BALL_RADIUS; //do not overwrite the wall
        return 1;
    }
    else return 0;
}

int right_wall_reached()
{
    if(xBall >= LCD_COL-1-BALL_RADIUS) //right wall reached
    {
        //xBall = LCD_COL-2-BALL_RADIUS; //do not overwrite right wall/racket
        return 1;
    }
    else return 0; //right wall not reached
}

int left_wall_reached()
{
    if(xBall <= BALL_RADIUS)  //left wall reached
    {
        xBall <= BALL_RADIUS; //do not overwrite wall/racket1
        return 1;
    }
    else return 0; //right wall not reached
}

int P1_racket_hit() //check ball vs left racket
{
 if( (yBall <= (yR1 + HALF_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+5) )
     return 1;
 else
     return 0;
}

int P1_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR1 + QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - QUARTER_RACKET_SIZE)) && (xBall <= BALL_RADIUS+5) )
     return 1;
 else
     return 0;
}
int P1_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR1 - QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+5) )
     return 1;
 else
     return 0;
}
int P1_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR1 + QUARTER_RACKET_SIZE)) && (yBall <= (yR1 + HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+5) )
     return 1;
 else
     return 0;
}

int P2_racket_hit() //check ball vs right racket
{
 if( (yBall <= (yR2 + HALF_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= (LCD_COL - BALL_RADIUS - 5)) )
     return 1;
 else
     return 0;
}

int P2_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR2 + QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - QUARTER_RACKET_SIZE)) && (xBall >= BALL_RADIUS-5) )
     return 1;
 else
     return 0;
}
int P2_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR2 - QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-5) )
     return 1;
 else
     return 0;
}
int P2_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR2 + QUARTER_RACKET_SIZE)) && (yBall <= (yR2 + HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-5) )
     return 1;
 else
     return 0;
}

void racket_movement_effect(int player){
    if(player == 1){
        if(yR1_old == yR1){ // racket not moving
            // When racket is not moving, the areas of the racket come into effect
            if(P1_racket_centre_hit()){ // hitting the centre of a racket will have negligible effect on changing ball y_displacement
               y_displacement = y_displacement;
            }
            if(P1_racket_upper_hit()){// hitting the upper part of a racket will increase upward direction ball y_displacement
               y_displacement = y_displacement - 1;
            }
            if(P1_racket_lower_hit()){// hitting the lower part of a racket will increase downward direction ball y_displacement
               y_displacement = y_displacement +1;
            }
        }
        // for a moving racket, instead of using racket zones, use the direction of racket movement to add to the ball's movement in the same direction
        if(yR1_old < yR1) // racket is moving down
            y_displacement = y_displacement+1; // move ball downward too
        if(yR1_old > yR1) // racket is moving up
            y_displacement = y_displacement-1; // move ball upward too
    }
    if(player == 2){
        if(yR2_old == yR2){ // racket not moving
            // When racket is not moving, the areas of the racket come into effect
            if(P2_racket_centre_hit()){// hitting the centre of a racket will have negligible effect on changing ball y_displacement
               y_displacement = y_displacement;
            }
            if(P2_racket_upper_hit()){// hitting the upper part of a racket will increase upward direction ball y_displacement
               y_displacement = y_displacement - 1;
            }
            if(P2_racket_lower_hit()){// hitting the lower part of a racket will increase downward direction ball y_displacement
               y_displacement = y_displacement +1;
            }
        }
        // for a moving racket, instead of using racket zones, use the direction of racket movement to add to the ball's movement in the same direction
        if(yR2_old < yR2) // racket is moving down
           y_displacement = y_displacement+1; // move ball downward too
        if(yR2_old > yR2) // racket is moving up
           y_displacement = y_displacement-1; // move ball upward too
    }
}

// Constructs the winning score string, later printed to LCD
void updateWinningScoreString(volatile char* winningString, int scorer){

    winningString[0]= 'P';
    winningString[1]= 'L';
    winningString[2]= 'A';
    winningString[3]= 'Y';
    winningString[4]= 'E';
    winningString[5]= 'R';
    winningString[6]= scorer + '0';
    winningString[7]= ' ';
    winningString[8]= 'W';
    winningString[9]= 'I';
    winningString[10]= 'N';
    winningString[11]= 'S';
    winningString[12]= '\0';
}

// Constructs the scorer score string, later printed to LCD
void updateScoreString(volatile char* scoreString, int scorer){

    scoreString[0]= ' ';
    scoreString[1]= 'G';
    scoreString[2] = 'O';
    scoreString[3]  = 'A';
    scoreString[4]= 'L';
    scoreString[5]= ' ';
    scoreString[6]= 'P';
    scoreString[7]= 'L';
    scoreString[8]= 'A';
    scoreString[9]= 'Y';
    scoreString[10]= 'E';
    scoreString[11]= 'R';
    scoreString[12]= scorer + '0';
    scoreString[13]= '\0';
}

// Constructs the current scores string, later printed to LCD
void updateCurrentScoresString(volatile char* currentScoresString, volatile int p1score, volatile int p2score){

    currentScoresString[0] = ' ';
    currentScoresString[1]= ' ';
    currentScoresString[2]= 'P';
    currentScoresString[3]= '1';
    currentScoresString[4]= ':';
    currentScoresString[5]= ' ';
    currentScoresString[6]= p1Score+'0';
    currentScoresString[7]= ' ';
    currentScoresString[8]= ' ';
    currentScoresString[9]= 'P';
    currentScoresString[10]= '2';
    currentScoresString[11]= ':';
    currentScoresString[12]= ' ';
    currentScoresString[13]= p2Score+'0';
    currentScoresString[14]= '\0';

}

//Update the state and position of the ball
//(CPU is awaken by TimerA1 interval ints)
void ball_update(void)
{


 //calculate new position and bouncing
 switch(ballStateInstance)
 {
  case INTRO: // Game intro is happening, e.g. menu selection
      halLcdClearScreen(); //CLEAR SCREEN

      halLcdPrintLine("Choose play mode:", 1, OVERWRITE_TEXT);//PRINT MESSAGE ......... NB ADD SCORES TO THE SCREEN
      halLcdPrintLine("JOYSTICK UP", 3, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("==2 player==", 4, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("JOYSTICK DOWN", 6, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("==vs AI==", 7, OVERWRITE_TEXT);//PRINT MESSAGE
      //stop TimerA1. This prevents new LCD and ball updates
      //but user input is operational as Port2 interrupts can still be triggered
      TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111

      // Now send CPU to sleep and wait for user to decide play mode
      // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
      __bis_SR_register(LPM3_bits + GIE);
      __no_operation(); //for debug
     break;

  case STARTING: //"Start" state, init ball position
          yBall = LCD_ROW >> 1;
          xBall = LCD_COL >> 1;
          if(x_displacement == NULL)
              x_displacement = +1; // if not set, set to +1 to give an initial direction of travel
          y_displacement = 0;
          ballStateInstance = MOVING; // begin moving
          break;

  case MOVING: //moving in free space

          xBall = xBall + x_displacement;
          yBall = yBall + y_displacement;

          if(top_wall_reached()){
              y_displacement *= -1; // flip the y direction of the ball
          }

          if(bottom_wall_reached()){
              y_displacement *= -1; // flip the y direction of the ball
          }

          if(P1_racket_hit()){
              x_displacement = +1; // 'bounce' the ball off the racket and keep it moving toward opposite side
              racket_movement_effect(1); // check to see if racket is moving or not, and add to the ball's direction displacement depending on this
          }

          if(P2_racket_hit()){
              x_displacement = -1;// 'bounce' the ball off the racket and keep it moving toward opposite side
              racket_movement_effect(2); // check to see if racket is moving or not, and add to the ball's direction displacement depending on this
          }

          //check left and right wall strikes
          if(left_wall_reached()){
              Scorer =  PLAYER2;
              p2Score += 1; // Increment player2 score
              updateScoreString(&scoreString,2); // Update the score string with the latest player who scored
              ballStateInstance = SCORING;
              x_displacement = -1; // When play restarts ball will go toward P2
          }
          if(right_wall_reached()){
              Scorer =  PLAYER1;
              p1Score += 1; // Increment player1 score
              updateScoreString(&scoreString,1); // Update the score string with the latest player who scored
              ballStateInstance = SCORING;
              x_displacement = +1; // When play restarts ball will go toward P1
          }

          break;
  case SCORING:

          //Handle scoring and winning
          halLcdClearScreen(); //CLEAR SCREEN
          switch(Scorer){
          case PLAYER1:
              if(p1Score >= winningScore){
                  updateWinningScoreString(winningString,1);
                  halLcdPrintLine(winningString, 1, OVERWRITE_TEXT);//PRINT MESSAGE
                  ballStateInstance = WINNING;
              }
              else{
                  halLcdPrintLine(scoreString, 1, OVERWRITE_TEXT);//PRINT current scorer as scoring a goal
              }
              break;
          case PLAYER2:
              if(p2Score >= winningScore){
                  updateWinningScoreString(winningString,2);
                  halLcdPrintLine(winningString, 1, OVERWRITE_TEXT);//PRINT MESSAGE
                  ballStateInstance = WINNING;
              }
              else{
                  halLcdPrintLine(scoreString, 1, OVERWRITE_TEXT);//PRINT current scorer as scoring a goal
              }
              break;
          }

          updateCurrentScoresString(currentScoresString,p1Score,p2Score); // Update the current scores string with current player scores
          halLcdPrintLine(currentScoresString,3,OVERWRITE_TEXT);  // print to LCD

          halLcdPrintLine(" Press an input   or Reset btn", 4, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("to continue", 6, OVERWRITE_TEXT);//PRINT MESSAGE
          //stop TimerA1. This prevents new LCD and ball updates
          //but user input is operational thanks to Port2 interrupts
          TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111

          // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
          __bis_SR_register(LPM3_bits + GIE);
          __no_operation(); //for debug

          /*MIGHT WANT TO THINK ABOUT HAVING A TIMEOUT BEFORE IT CONTINUES BY ITSELF BACK TO START*/
          /*
           * START TIMER AND GO INTO LPM
           *
           * CREATE TIMEOUT ISR WHICH SETS STATE TO STARTING
           *
           * */

         break;

 }

}
