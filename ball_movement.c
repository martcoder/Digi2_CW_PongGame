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
        xBall = 1+BALL_RADIUS; //do not overwrite wall/racket1
        return 1;
    }
    else return 0; //right wall not reached
}

int P1_racket_hit() //check ball vs left racket
{
 if( (yBall <= (yR1 + HALF_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall < BALL_RADIUS) )
     return 1;
 else
     return 0;
}

//Update the state and position of the ball
//(CPU is awaken by TimerA1 interval ints)
void ball_update(void)
{


 //calculate new position and bouncing
 switch(ballStateInstance)
 {
  case STARTING: //"Start" state, init ball position
          yBall = LCD_ROW >> 1;
          xBall = LCD_COL >> 1;
          x_displacement = +1;
          y_displacement = 0;
          ballStateInstance = MOVING; // begin moving
          break;

  case MOVING: //moving in free space

          xBall = xBall + x_displacement;
          yBall = yBall + y_displacement;

          if(top_wall_reached()){
              y_displacement = +1;

          }

          if(bottom_wall_reached()){
              y_displacement = -1;

          }

          //check right wall bounce
          if(right_wall_reached()){
              x_displacement = -1;  //Right wall is here, bounce to direction 8

          }

          if(P1_racket_hit()){
              x_displacement = +1;

          }
/*
          if(left_wall_reached()){
              ballStateInstance = SCORING;

          }*/


          break;
  case SCORING: //stopped
          /*xBall = xBall + 2;
          yBall = yBall - 1;

          // check if left wall hit
          if(left_wall_reached())


          //check right wall bounce
          if(right_wall_reached())
              x_displacement = -1;  //Right wall is here, bounce to direction 8

          // check paddle hit
          if()

          xBall = xBall - 2;
          //check left wall reached
          if(left_wall_reached())
          {
            //If racket is here bounce, otherwise it's a goal
            if(P1_racket_hit())
            {
               if(R1Dir == UP) //up effect on ball
               { ballState = 2; }
               else
               {
                   if(R1Dir == DOWN) //down effect on ball
                    {   ballState = 14; }
                   else //no effect, normal bounce
                    {   ballState = 1; }
               }
            }
            else //Goal! Player 1 missed the ball
             { ballState = 16; }
          }
          break;*/

          break;
  case 15://Right-hand player missed the ball!

          break;
  case 16://Left-hand player missed the ball!
          //A very simplistic game end handling
          halLcdClearScreen(); //CLEAR SCREEN
          halLcdPrintLine("     GOAL", 4, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine(" Reset to start", 6, OVERWRITE_TEXT);//PRINT MESSAGE
          //stop TimerA1. This prevents new LCD and ball updates
          //but user input is operational because is driven by TimerB0
          TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111
          break;
 }


}
