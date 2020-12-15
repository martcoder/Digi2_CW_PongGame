/*********************************************************
 * game_mechanics.c
 *
 * Implements the state diagram for the ball's movement
 * Only a few states are completed, refer to the CW description
 * to find out about the remaining states
 *
 * Created on: Nov 2017
 * Author: M MATA (GCU)
 *********************************************************/
#include <game_mechanics.h>
#include "general_settings.h"
#include "hal_lcd.h"
#include "msp430f5438a.h"
#include "lookup.c"



int top_wall_reached()
{
    if(yBall <= (BALL_RADIUS+12) ) //top wall reached
    {
        yBall = 1+BALL_RADIUS+12; //do not overwrite the wall
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
 if( (yBall <= (yR1 + HALF_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}

int P1_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR1 + QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - QUARTER_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}
int P1_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR1 - QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}
int P1_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR1 + QUARTER_RACKET_SIZE)) && (yBall <= (yR1 + HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}

int P2_bonus_racket_hit() //check ball vs player2 bonus racket
{
 if( (yBall <= (yR2bonus + QUARTER_RACKET_SIZE)) && (yBall >= (yR2bonus - QUARTER_RACKET_SIZE)) && (xBall >= (xR2bonus - BALL_RADIUS - 5)) ){
     // for a moving racket, instead of using racket zones, use the direction of racket movement to add to the ball's movement in the same direction
     if(yR2bonus_old < yR2bonus) // racket is moving down
          y_displacement = y_displacement+1; // move ball downward too
     if(yR2bonus_old > yR2bonus) // racket is moving up
          y_displacement = y_displacement-1; // move ball upward too
     return 1;
 }
 else
     return 0;
}

int P2_racket_hit() //check ball vs right racket
{
 if( (yBall <= (yR2 + HALF_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= (LCD_COL - BALL_RADIUS - 4)) )
     return 1;
 else
     return 0;
}

int P2_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR2 + QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - QUARTER_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
     return 1;
 else
     return 0;
}
int P2_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR2 - QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
     return 1;
 else
     return 0;
}
int P2_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR2 + QUARTER_RACKET_SIZE)) && (yBall <= (yR2 + HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
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
        if(AI_enabled){
            if(toggle_AI_direction > 0)
                y_displacement = y_displacement-1; // move ball upward too
            if(toggle_AI_direction < 0)
                y_displacement = y_displacement+1; // move ball downward too
        }
        else{
            if(yR2_old < yR2) // racket is moving down
               y_displacement = y_displacement+1; // move ball downward too
            if(yR2_old > yR2) // racket is moving up
               y_displacement = y_displacement-1; // move ball upward too
        }
    }
}

void clearProjectiles(){

    // Clear old projectiles
        halLcdHLine(p1ProjectileA_X - PROJECTILE_HALF_SIZE, p1ProjectileA_X + PROJECTILE_HALF_SIZE, p1ProjectileA_Y, PIXEL_OFF);
                      halLcdHLine(p1ProjectileB_X -PROJECTILE_HALF_SIZE, p1ProjectileB_X + PROJECTILE_HALF_SIZE, p1ProjectileB_Y, PIXEL_OFF);
                      halLcdHLine(p1ProjectileA_X_old - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old, PIXEL_OFF);
                      halLcdHLine(p1ProjectileB_X_old -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old, PIXEL_OFF);
                      halLcdHLine(p1ProjectileA_X_old2 - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old2, PIXEL_OFF);
                      halLcdHLine(p1ProjectileB_X_old2 -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old2, PIXEL_OFF);

}

void resetProjectiles(){

    // Set projectile A position off screen for now so as to not interfere with the ball
    p1ProjectileA_X = xR1-10;
    p1ProjectileA_Y = yR1 - HALF_RACKET_SIZE;


    // Update previous/old for projectile A
    p1ProjectileA_X_old = p1ProjectileA_X;
    p1ProjectileA_X_old2 = p1ProjectileA_X;
    p1ProjectileA_Y_old = p1ProjectileA_Y;
    p1ProjectileA_Y_old2 = p1ProjectileA_Y;


    // Set projectile B position off screen for now so as to not interfere with the ball
                                p1ProjectileB_X = xR1-10;
                                p1ProjectileB_Y = yR1 + HALF_RACKET_SIZE;



    // Update previous/old for projectile B
    p1ProjectileB_X_old = p1ProjectileB_X;
    p1ProjectileB_X_old2 = p1ProjectileB_X;
    p1ProjectileB_Y_old = p1ProjectileB_Y;
    p1ProjectileB_Y_old2 = p1ProjectileB_Y;

    p1Projectiles_onscreen = 0;
}

// Place projectiles at racket cannons ready to move
void powerUpProjectiles(){

    p1ProjectileA_X = xR1+10;
    p1ProjectileB_X = xR1+10;
}

void updateBannerString(volatile char* bannerString,int p1score, int p2score, int p1bonusEnabled, int p2bonusEnabled){

    if(p1bonusEnabled == 1)
        bannerString[0] = 'B';
    else
        bannerString[0] = ' ';
    bannerString[1] = ' ';
    bannerString[2] = 'P';
    bannerString[3] = '1';
    bannerString[4] = ':';
    bannerString[5] = p1score+'0';
    bannerString[6] = ' ';
    bannerString[7] = 'v';
    bannerString[8] = 's';
    bannerString[9] = ' ';
    if(AI_enabled == 1){
     bannerString[10] = 'A';
     bannerString[11] = 'I';
    }
    else{
     bannerString[10] = 'P';
     bannerString[11] = '2';
    }
    bannerString[12] = ':';
    bannerString[13] = p2score+'0';
    bannerString[14] = ' ';
    if(p2bonusEnabled == 1)
        bannerString[15] = 'B';
    else
        bannerString[15] = ' ';
    bannerString[16] = '\0';
}

// Constructs the winning score string, later printed to LCD
void updateWinningScoreString(volatile char* winningString, int scorer){

    if( (AI_enabled == 1) && (scorer == 2) ){
        winningString[0]= ' ';
        winningString[1]= ' ';
        winningString[2]= 'A';
        winningString[3]= 'I';
        winningString[4]= ' ';
        winningString[5]= ' ';
        winningString[6]= ' ';
    }
    else{
        winningString[0]= 'P';
        winningString[1]= 'L';
        winningString[2]= 'A';
        winningString[3]= 'Y';
        winningString[4]= 'E';
        winningString[5]= 'R';
        winningString[6]= scorer + '0';
    }
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
    if((AI_enabled == 1) && (scorer == 2) ){
        scoreString[6]= ' ';
        scoreString[7]= ' ';
        scoreString[8]= 'A';
        scoreString[9]= 'I';
        scoreString[10]= ' ';
        scoreString[11]= ' ';
        scoreString[12]= ' ';
    }
    else{
        scoreString[6]= 'P';
        scoreString[7]= 'L';
        scoreString[8]= 'A';
        scoreString[9]= 'Y';
        scoreString[10]= 'E';
        scoreString[11]= 'R';
        scoreString[12]= scorer + '0';
    }
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
    if((AI_enabled == 1)){
        currentScoresString[9]= 'A';
        currentScoresString[10]= 'I';
    }
    else{
        currentScoresString[9]= 'P';
        currentScoresString[10]= '2';
    }
    currentScoresString[11]= ':';
    currentScoresString[12]= ' ';
    currentScoresString[13]= p2Score+'0';
    currentScoresString[14]= '\0';

}

//Update the state and position of the ball
//(CPU is awaken by TimerA1 interval ints)
void game_update(void)
{

    switch(BonusStatus){
    case INPLAY:


        // If ball meets bonus, enable bonus for that player...
        if(
                      (
                              ( xBall <= ( BONUS_APPEAR_X + 5 ) )
                              &&
                              ( xBall >= ( BONUS_APPEAR_X - 5 ) )
                      )
                      &&
                      (
                              ( yBall <= ( BONUS_APPEAR_Y + 5 ) )
                              &&
                              ( yBall >= ( BONUS_APPEAR_Y - 5) )
                       )
          ){
            BonusStatus = PICKEDUP;

                    if(LastHitterInstance ==  P1){ //player1 struck ball last
                        p1bonusEnabled = 1;
                        p1bonusCooldown = 2;
                         // don't draw the bonus now that it has been claimed
                    }
                    if(LastHitterInstance ==  P2){ // player2 or AI struck ball last
                        p2bonusEnabled = 1;
                        p2bonusCooldown = 2;
                        //Draw the bonusracket2
                        halLcdVLine(xR2bonus-3, yR2bonus - QUARTER_RACKET_SIZE, yR2bonus + QUARTER_RACKET_SIZE, PIXEL_ON);
                        halLcdVLine(xR2bonus- 4, yR2bonus - QUARTER_RACKET_SIZE, yR2bonus + QUARTER_RACKET_SIZE, PIXEL_ON);
                    }

            // Also update the banner to show that bonus has been picked up
            updateBannerString(bannerString,p1Score,p2Score,p1bonusEnabled,p2bonusEnabled);
            halLcdPrintLine(bannerString, 0, OVERWRITE_TEXT);//PRINT MESSAGE
          }

        break;
    case PICKEDUP:


        // If the ball meets the projectile, destroy projectile, then flip the ball x and y directions
        if(
              (
                      ( xBall <= ( p1ProjectileA_X + (PROJECTILE_HALF_SIZE << 1) ) )
                      &&
                      ( xBall >= ( p1ProjectileA_X - (PROJECTILE_HALF_SIZE << 1) ) )
              )
              &&
              (
                      ( yBall <= ( p1ProjectileA_Y + (PROJECTILE_HALF_SIZE << 2)  ) )
                      &&
                      ( yBall >= ( p1ProjectileA_Y - (PROJECTILE_HALF_SIZE << 2)  ) )
               )
               &&
               (p1Projectiles_onscreen = 1)
          ){
            p1ProjectileA_X = LCD_COL+10; //set to just off screen to avoid affecting the ball anymore this time around

            clearProjectiles();
            p1Projectiles_onscreen = 0;


            x_displacement *= -1;
            y_displacement *- -1;
        }
        // If the ball meets the projectile, destroy the projectile, and flip the ball displacement
        if(
             (
                 ( xBall <= ( p1ProjectileB_X + (PROJECTILE_HALF_SIZE << 1) ) )
                 &&
                 ( xBall >= ( p1ProjectileB_X - (PROJECTILE_HALF_SIZE << 1) ) )
             )
             &&
             (
                 ( yBall <= ( p1ProjectileB_Y + (PROJECTILE_HALF_SIZE << 2) ) )
                 &&
                 ( yBall >= ( p1ProjectileB_Y - (PROJECTILE_HALF_SIZE << 2) ) )
             )
         ){
            p1ProjectileB_X = LCD_COL+10;

            clearProjectiles();

            x_displacement *= -1;
            y_displacement *- -1;
        }
        break;
    case OUTOFPLAY:

    }

 //calculate new position and bouncing
 switch(gameStateInstance)
 {
 case LOADING:

           halLcdClearScreen(); //CLEAR SCREEN

           halLcdPrintLine("Welcome Back To:", 1, OVERWRITE_TEXT);//PRINT MESSAGE ......... NB ADD SCORES TO THE SCREEN
           halLcdPrintLine("The Days Of...:", 2, OVERWRITE_TEXT);//PRINT MESSAGE
           halLcdPrintLine("=====P     N  ====", 4, OVERWRITE_TEXT);//PRINT MESSAGE
           halLcdPrintLine("=====   O     G===", 4, OVERWRITE_TEXT);//PRINT MESSAGE
           halLcdPrintLine("Move JOYSTICK UP", 7, OVERWRITE_TEXT);//PRINT MESSAGE

           //stop TimerA1. This prevents new LCD and ball updates
                 //but user input is operational as Port2 interrupts can still be triggered
                 TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111
                 // Now send CPU to sleep and wait for user to proceed
                       // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
                       __bis_SR_register(LPM3_bits + GIE);
                       __no_operation(); //for debug

     break;
  case INTRO: // Game intro is happening, e.g. menu selection

      //P2IE &= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2); //disable pins interrupts temporarily

      halLcdClearScreen(); //CLEAR SCREEN

      halLcdPrintLine("Choose play mode:", 1, OVERWRITE_TEXT);//PRINT MESSAGE ......... NB ADD SCORES TO THE SCREEN
      halLcdPrintLine("JOYSTICK UP", 3, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("==2 player==", 4, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("JOYSTICK DOWN", 6, OVERWRITE_TEXT);//PRINT MESSAGE
      halLcdPrintLine("==vs AI==", 7, OVERWRITE_TEXT);//PRINT MESSAGE

      p1Score = 0;
      p2Score = 0;

      //TimerB0Init(); // initialise timerB as a way of providing a timeout and preventing further interrupts for some time

      // Now stop TimerB0
      //TB0CTL    = TBSSEL_1 + MC_0 + TBCLR + TBIE;


      //stop TimerA1. This prevents new LCD and ball updates
      //but user input is operational as Port2 interrupts can still be triggered
      TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111

      //P2IE |= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2); //Enable pins interrupts

      // Now send CPU to sleep and wait for user to decide play mode
      // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
      __bis_SR_register(LPM3_bits + GIE);
      __no_operation(); //for debug
     break;

  case STARTING: //"Start" state, init ball position

      // Ensure that rackets are in the correct position, if not move them ...
      if(yR1 < (LCD_ROW >> 1)){
          yR1_previousPosition = yR1;
          yR1 += 1; //move racket 1 pixels down
          R1Dir = DOWN;
      }

      if(yR1 > (LCD_ROW >> 1)){
          yR1_previousPosition = yR1;
          yR1 -= 1; //move racket 1 pixels up
          R1Dir = UP;
      }

      if(yR2 < (LCD_ROW >> 1)){
          yR2_previousPosition = yR2;
          yR2 += 1; //move racket 1 pixels down
          R2Dir = DOWN;
      }

      if(yR2 > (LCD_ROW >> 1)){
          yR2_previousPosition = yR2;
          yR2 -= 1; //move racket 1 pixels up
          R2Dir = UP;
      }

      if(yR2bonus < (LCD_ROW >> 1)){
                yR2bonus_previousPosition = yR2bonus;
                yR2bonus += 1; //move racket 1 pixels down
                R2bonusDir = DOWN;
      }

      if(yR2bonus > (LCD_ROW >> 1)){
                yR2bonus_previousPosition = yR2bonus;
                yR2bonus -= 1; //move racket 1 pixels up
                R2bonusDir = UP;
      }

      // When rackets are back in starting position, redraw them
      if( (yR1 == ( LCD_ROW >> 1 ) ) && (yR2 == (LCD_ROW >> 1 ) ) ){

          halLcdPrintXY( "                ", 10, 80, OVERWRITE_TEXT);
          //halLcdPrintLine("                     ", 6, OVERWRITE_TEXT);//PRINT blanks where scorer was printed

          //Draw new racket1
          halLcdVLine(xR1, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
          halLcdVLine(xR1 + 1, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
          //halLcdVLine(xR1 + 2, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);

          //Draw new racket2
          halLcdVLine(xR2, yR2 - HALF_RACKET_SIZE, yR2 + HALF_RACKET_SIZE, PIXEL_ON);
          halLcdVLine(xR2 - 1, yR2 - HALF_RACKET_SIZE, yR2 + HALF_RACKET_SIZE, PIXEL_ON);

          if(p2bonusEnabled == 1){
              //Draw the bonusracket2
              halLcdVLine(xR2bonus-3, yR2bonus - QUARTER_RACKET_SIZE, yR2bonus + QUARTER_RACKET_SIZE, PIXEL_ON);
              halLcdVLine(xR2bonus- 4, yR2bonus - QUARTER_RACKET_SIZE, yR2bonus + QUARTER_RACKET_SIZE, PIXEL_ON);
          }
      }

       // Initial position of the ball
          yBall = LCD_ROW >> 1;
          xBall = LCD_COL >> 1;
          if(x_displacement == NULL)
              x_displacement = +1; // if not set, set to +1 to give an initial direction of travel
          y_displacement = 0;

          updateBannerString(bannerString,p1Score,p2Score,p1bonusEnabled,p2bonusEnabled);
          halLcdPrintLine(bannerString, 0, OVERWRITE_TEXT);//PRINT MESSAGE

          //halLcdPrintLine("some scores", 1, OVERWRITE_TEXT);//PRINT MESSAGE
          if( (yR1 == (LCD_ROW >> 1)) && (yR2 == (LCD_ROW >> 1)) ) // if rackets in the correct starting position, proceed to next game state
              gameStateInstance = MOVING; // begin moving
          break;

  case MOVING: //moving objects in free space

         // first do projectiles if they are active....
      if(p1bonusEnabled && p1Projectiles_active){

          if( (p1Projectiles_pressed == 1) /*|| (p1Projectiles_onscreen == 0)*/ ) // if not on screen yet, give a starting position. Also resets start position if trigger is pressed again
          {
              clearProjectiles();
              resetProjectiles();
              powerUpProjectiles(); // This should only happen when trigger is pressed.

              p1Projectiles_pressed = 0; // Clear the pressed variable
              p1Projectiles_onscreen = 1; // Set variable to mark that player1 projectiles are now on-screen
          }


          // Move p1 projectile A position
          p1ProjectileA_X = p1ProjectileA_X + p1_projectileA_x_displacement;
          // Move p1 projectile B position
          p1ProjectileB_X = p1ProjectileB_X + p1_projectileB_x_displacement;

          if( ( p1ProjectileA_X > (LCD_COL-6) ) || ( p1ProjectileB_X > (LCD_COL-6) ) ){

              p1ProjectileA_X = LCD_COL+10; //set to just off screen to avoid affecting the ball anymore this time around

              clearProjectiles();
              resetProjectiles();
              p1Projectiles_active = 0;
              p1Projectiles_onscreen = 0;

          }

          p1Projectiles_pressed = 0;

      }



          xBall = xBall + x_displacement;
          yBall = yBall + y_displacement;

          if(top_wall_reached()){
              y_displacement *= -1; // flip the y direction of the ball
          }

          if(bottom_wall_reached()){
              y_displacement *= -1; // flip the y direction of the ball
          }

          if(P1_racket_hit()){
              LastHitterInstance = P1;
              x_displacement = +1; // 'bounce' the ball off the racket and keep it moving toward opposite side
              racket_movement_effect(1); // check to see if racket is moving or not, and add to the ball's direction displacement depending on this
              if(AI_enabled) // if AI is playing, change it's hitting movement direction
                                toggle_AI_direction /*= toggle_AI_direction;*/  *= -1;
          }

          // Check ball against player2 bonus racket
          if(p2bonusEnabled == 1){
              if(P2_bonus_racket_hit()){
                  LastHitterInstance = P2;
                  x_displacement = -1;// 'bounce' the ball off the racket and keep it moving toward opposite side

              }
          }

          if(P2_racket_hit()){
              LastHitterInstance = P2;
              x_displacement = -1;// 'bounce' the ball off the racket and keep it moving toward opposite side
              racket_movement_effect(2); // check to see if racket is moving or not, and add to the ball's direction displacement depending on this

          }

          //check left and right wall strikes
          if(left_wall_reached()){
              P2IE &= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2); //disable pins interrupts temprorarily
              Scorer =  PLAYER2;
              p2Score += 1; // Increment player2 score
              updateScoreString(scoreString,2); // Update the score string with the latest player who scored
              gameStateInstance = SCORING;
              x_displacement = -1; // When play restarts ball will go toward P2
          }
          if(right_wall_reached()){
              P2IE &= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2); //disable pins interrupts temprorarily
              Scorer =  PLAYER1;
              p1Score += 1; // Increment player1 score
              updateScoreString(scoreString,1); // Update the score string with the latest player who scored
              gameStateInstance = SCORING;
              x_displacement = +1; // When play restarts ball will go toward P1

          }

          break;
  case SCORING:

      // temporarily disable interrupts while information is printed...
      //P2IE &= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2);



          //Handle scoring and winning
          //halLcdClearScreen(); //CLEAR SCREEN
          switch(Scorer){
          case PLAYER1:

              if(p1Score == winningScore){
                  updateWinningScoreString(winningString,1);
                  halLcdPrintLine(winningString, 2, OVERWRITE_TEXT);//PRINT MESSAGE
                  gameStateInstance = WINNING;
              }
              else{
                  if(p1Score == bonusScore){// check if player score has reached the bonus score
                      BonusStatus = INPLAY;// set bonus to be in play
                  }
                  if(p2bonusCooldown > 0){ // decrement player2 bonus cooldown if opponent scores
                      p2bonusCooldown--;
                  }
                  else{ // switch off player2 bonus
                      p2bonusEnabled = 0;
                      //also clear the bonus racket from the LCD
                      halLcdVLine(xR2bonus-3, yR2bonus_previousPosition - QUARTER_RACKET_SIZE, yR2bonus_previousPosition + QUARTER_RACKET_SIZE, PIXEL_OFF);
                      halLcdVLine(xR2bonus- 4, yR2bonus_previousPosition - QUARTER_RACKET_SIZE, yR2bonus_previousPosition + QUARTER_RACKET_SIZE, PIXEL_OFF);
                  }
                  //halLcdPrintLine(scoreString, 1, OVERWRITE_TEXT);//PRINT current scorer as scoring a goal
                  updateBannerString(bannerString,p1Score,p2Score,p1bonusEnabled,p2bonusEnabled);
                  halLcdPrintLine(bannerString, 0, OVERWRITE_TEXT);//PRINT MESSAGE
                  halLcdPrintLine(winningString, 2, OVERWRITE_TEXT);//PRINT MESSAGE

                  updateScoreString(scoreString,1);
                  halLcdPrintXY( scoreString, 10, 80, OVERWRITE_TEXT);
                  //halLcdPrintLine(scoreString, 6, OVERWRITE_TEXT);//PRINT current scorer as scoring a goal

                  gameStateInstance = STARTING;
                  //halLcdPrintLine("some p1 scores", 1, OVERWRITE_TEXT);//PRINT MESSAGE
              }
              break;
          case PLAYER2:

              if(p2Score == winningScore){
                  updateWinningScoreString(winningString,2);
                  halLcdPrintLine(winningString, 2, OVERWRITE_TEXT);//PRINT MESSAGE
                  gameStateInstance = WINNING;
              }
              else{

                  if(p2Score == bonusScore){// check if player score has reached the bonus score
                     BonusStatus = INPLAY; // set bonus to be in play
                  }
                  if(p1bonusCooldown > 0){ // decrement player1 bonus cooldown if opponent scores
                     p1bonusCooldown--;
                  }
                  else{ // switch off player1 bonus
                      p1bonusEnabled = 0;
                  }

                  updateBannerString(bannerString,p1Score,p2Score,p1bonusEnabled,p2bonusEnabled);
                  halLcdPrintLine(bannerString, 0, OVERWRITE_TEXT);//PRINT MESSAGE

                  updateScoreString(scoreString,2);
                  halLcdPrintXY( scoreString, 10, 80, OVERWRITE_TEXT);
                  //halLcdPrintLine(scoreString, 6, OVERWRITE_TEXT);//PRINT current scorer as scoring a goal

                  gameStateInstance = STARTING;
                  //halLcdPrintLine("some p2 scores", 1, OVERWRITE_TEXT);//PRINT MESSAGE
              }
              break;
          }



          break;

          case WINNING:

              halLcdPrintLine("   GAME OVER    ", 0, OVERWRITE_TEXT);//Overwrite banner with 'GAME OVER'
              updateCurrentScoresString(currentScoresString,p1Score,p2Score); // Update the current scores string with current player scores
              halLcdPrintLine(currentScoresString,4,OVERWRITE_TEXT);  // print to LCD

              halLcdPrintLine(" Press an input   or Reset btn", 5, OVERWRITE_TEXT);//PRINT MESSAGE
              halLcdPrintLine(" to continue", 7, OVERWRITE_TEXT);//PRINT MESSAGE



          //TimerB0Init(); // initialise timerB as a way of providing a timeout and preventing further interrupts for some time

          // Now stop TimerB0
          //TB0CTL    = TBSSEL_1 + MC_0 + TBCLR + TBIE;
/*
          //stop TimerA1. This prevents new LCD and ball updates
          //but user input is operational thanks to Port2 interrupts
          TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111

          //Re-enable interrupts
          //P2IE |= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2);

          // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
          __bis_SR_register(LPM3_bits + GIE);
          __no_operation(); //for debug
*/

          /*MIGHT WANT TO THINK ABOUT HAVING A TIMEOUT BEFORE IT CONTINUES BY ITSELF BACK TO START*/
          /*
           * START TIMER AND GO INTO LPM
           *
           * CREATE TIMEOUT ISR WHICH SETS STATE TO STARTING
           *
           * */

         break;

 }
    // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode
    __bis_SR_register(LPM3_bits + GIE);
    __no_operation(); //for debug

}
