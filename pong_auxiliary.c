#include "pong_auxiliary.h"

/*
 * Move AI racket toward the y-value of the ball
 * unless the ball is very close to the racket in which case do a 'human movement' to send the ball off at an angle
 * */
void ai_movement(void){
    if(newAIPositionsDrawn ==1){
        newAIPositionsDrawn = 0;

        // if AI racket not hitting top wall, and also lower than the ball, move racket up, unless ball is close to racket
        if( (xBall < (LCD_COL - 6)) && (yR2 > (HALF_RACKET_SIZE+13)) && (yR2 > yBall) ){
            yR2_previousPosition = yR2;
            //yR2_old = yR2;
            yR2 -= 1; //move racket 1 pixels up
            R2Dir = UP;

            // Also adjust the bonus racket position if it is active
            if(p2bonusEnabled == 1){
                if ( yR2bonus < (LCD_ROW - QUARTER_RACKET_SIZE)  ) //avoid going lower than bottom of LCD
                {
                    yR2bonus_previousPosition = yR2bonus;
                    //yR2bonus_old = yR2bonus;
                    yR2bonus += 1; //move racket 1 pixels down (opposite of player2 standard racket)
                }
                R2bonusDir = DOWN;
            }

            return;
        }
        // if AI racket not hitting bottom wall, and also higher than ball, move racket down, unless ball is close to racket
        if( (xBall < (LCD_COL - 6)) && (yR2 < (LCD_ROW - HALF_RACKET_SIZE)) && (yR2 < yBall) ){
            yR2_previousPosition = yR2;
            //yR2_old = yR2;
            yR2 += 1; //move racket 1 pixels down
            R2Dir = DOWN;

            if(p2bonusEnabled == 1){
              if ( yR2bonus > (QUARTER_RACKET_SIZE + 13)  ) //avoid going higher than top wall
              {
                   yR2bonus_previousPosition = yR2bonus;
                   //yR2bonus_old = yR2bonus;
                   yR2bonus -= 1; //move racket 1 pixels up (opposite of player2 standard racket)
              }
              R2bonusDir = UP;
            }

            return;
        }
        // check racket is not against ceiling, then 'human movement' upward if ball is close to the racket
        if(xBall > (LCD_COL - 6) && (yR2 > (HALF_RACKET_SIZE+13)) ){
            if(toggle_AI_direction > 0){
                yR2_previousPosition = yR2;
                //yR2_old = yR2;
                yR2 -= 1; //move racket 1 pixels up
                R2Dir = UP;
                return;
            }

         }
         // if racket is not against the floor, 'human movement' downward if ball is close to the racket
         if( (xBall > (LCD_COL - 6)) && (yR2 < (LCD_ROW - HALF_RACKET_SIZE)))
         {
             if(toggle_AI_direction < 0){
                yR2_previousPosition = yR2;
                //yR2_old = yR2;
                yR2 += 1; //move racket 1 pixels down
                R2Dir = DOWN;
                return;
             }
          }
    }
}

/*
 * Check if player inputs are being pressed, and adjust racket positions accordingly
 * */
void UserInputs_update(void)
{
  if(newPositionsDrawn == 1){

    newPositionsDrawn = 0;

    ContinuousPressChecker = 0; // This will be set below if an input is being pressed
    R1Dir = STOP;
    R2Dir = STOP;
    //EXAMPLE: read button SW1

    //Check individual flags to find out if each direction was activated
    if(!(P2IN & BIT5)){ //DOWN pressed for Player1
        ContinuousPressChecker = 1; // Continue to check if this is continously held down
        if (yR1 < (LCD_ROW - HALF_RACKET_SIZE) ) //avoid overwriting top wall
        {
           yR1_previousPosition = yR1;
           yR1 += 1; //move racket 1 pixels down
        }
        R1Dir = DOWN;
        return;
    }
    if(!(P2IN & BIT4)){ //UP pressed for Player1
        ContinuousPressChecker = 1; // Continue to check if this is continously held down
        if (yR1 > (HALF_RACKET_SIZE+13)) //avoid overwriting top wall
        {
           yR1_previousPosition = yR1;
           yR1 -= 1; //move racket 1 pixel up
        }
        R1Dir = UP;
        return;
    }

    if(!(P2IN & BIT2)){ //RIGHT pressed for Player1, this means FIRE projectile when bonus enabled for this player
            //ContinuousPressChecker = 1; // Continue to check if this is continously held down
            // Make this player's projectiles active!!!
            p1Projectiles_pressed = 1;
            p1Projectiles_active = 1;

            return;
        }

    if(!(P2IN & BIT6)) //SW1 pressed for UP for Player2
    {
          ContinuousPressChecker = 1; // Continue to check if this is continously held down
          if (yR2 > (HALF_RACKET_SIZE+13)) //avoid overwriting top wall
          {
              yR2_previousPosition = yR2;
              yR2 -= 1; //move racket 1 pixels up
          }

          R2Dir = UP;

          if(p2bonusEnabled == 1){
              if ( yR2bonus < (LCD_ROW - QUARTER_RACKET_SIZE)  ) //avoid going lower than bottom of LCD
              {
                  yR2bonus_previousPosition = yR2bonus;
                  yR2bonus += 1; //move racket 1 pixels down (opposite of player2 standard racket)
              }
              R2bonusDir = DOWN;
          }

          return;
    }

    if(!(P2IN & BIT7)) //SW2 pressed for DOWN for Player2
    {
          ContinuousPressChecker = 1; // Continue to check if this is continously held down
          if (yR2 < (LCD_ROW - HALF_RACKET_SIZE) ) //avoid overwriting top wall
          {
             yR2_previousPosition = yR2;
             yR2 += 1; //move racket 1 pixels down
          }
          R2Dir = DOWN;

          if(p2bonusEnabled == 1){
              if ( yR2bonus > (QUARTER_RACKET_SIZE+12) ) //avoid overwriting top wall
              {
                 yR2bonus_previousPosition = yR2bonus;
                 yR2bonus -= 1; //move racket 1 pixels up (opposite of player2 standard racket)
              }
              R2bonusDir = UP;
          }

          return;
     }

  }
}

//Update drawings in LCD screen (CPU is woken after 9 TimerA1 intervals)
void LCD_update(void)
{

     /*update older positions to clear old rackets and draw new ones
    * NB: this has been optimised to only draw the new top line and undraw the old bottom line for moving UP,
    * and vice versa for moving DOWN, rather than undrawing then drawing the entire racket(s) again.
    */

     //Clear oldest ball
     halLcdCircle(xBall_old2, yBall_old2, BALL_RADIUS, PIXEL_OFF);
     halLcdPixel(xBall_old2, yBall_old2, PIXEL_OFF);

     //Draw ball trail
     halLcdCircle(xBall_old, yBall_old, BALL_RADIUS, PIXEL_ON);
     halLcdPixel(xBall_old, yBall_old, PIXEL_ON);

     //Draw new ball
     halLcdCircle(xBall, yBall, BALL_RADIUS, PIXEL_ON);
     halLcdPixel(xBall, yBall, PIXEL_ON);
     //update older positions for drawing ball trail and deleting old ball
     xBall_old2=xBall_old;
     yBall_old2=yBall_old;
     xBall_old=xBall;
     yBall_old=yBall;

     if( (BonusStatus == INPLAY) && (bonusDrawn == 0) ){
         // Draw bonus
         halLcdCircle( BONUS_APPEAR_X, BONUS_APPEAR_Y, BONUS_RADIUS, PIXEL_ON);//draw bonus
         bonusDrawn = 1;
     }
     // If bonus has been drawn and now has been hit by a player, it needs to be undrawn
     if( (BonusStatus == PICKEDUP ) && (bonusDrawn == 1) ){
         halLcdCircle( BONUS_APPEAR_X, BONUS_APPEAR_Y, BONUS_RADIUS, PIXEL_OFF);//undraw bonus
         bonusDrawn = 0; // clear the bonus drawn variable
     }

     // Undraw and draw projectile positions
     if( (p1bonusEnabled == 1) && (p1Projectiles_onscreen == 1)  ){

         // Clear old projectiles
         halLcdHLine(p1ProjectileA_X_old2 - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old2, PIXEL_OFF);
         halLcdHLine(p1ProjectileB_X_old2 -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old2, PIXEL_OFF);

         // Draw projectile trails
         halLcdHLine(p1ProjectileA_X_old-PROJECTILE_HALF_SIZE, p1ProjectileA_X_old+PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old, PIXEL_ON);
         halLcdHLine(p1ProjectileB_X_old-PROJECTILE_HALF_SIZE, p1ProjectileB_X_old +PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old, PIXEL_ON);

         // Draw new projectile positions
         halLcdHLine(p1ProjectileA_X-PROJECTILE_HALF_SIZE, p1ProjectileA_X+PROJECTILE_HALF_SIZE, p1ProjectileA_Y , PIXEL_ON);
         halLcdHLine(p1ProjectileB_X-PROJECTILE_HALF_SIZE, p1ProjectileB_X+PROJECTILE_HALF_SIZE, p1ProjectileB_Y, PIXEL_ON);

         // Update older positions
         p1ProjectileA_X_old2 = p1ProjectileA_X_old;
         p1ProjectileA_Y_old2 = p1ProjectileA_Y_old;
         p1ProjectileA_X_old = p1ProjectileA_X;
         p1ProjectileA_Y_old = p1ProjectileA_Y;

         p1ProjectileB_X_old2 = p1ProjectileB_X_old;
         p1ProjectileB_Y_old2 = p1ProjectileB_Y_old;
         p1ProjectileB_X_old = p1ProjectileB_X;
         p1ProjectileB_Y_old = p1ProjectileB_Y;
     }

     // Undraw old racket positions, draw new positions
     if(R1Dir == UP){
         //clear old racket1 bottom line
         halLcdHLine(xR1_old, xR1_old+2, yR1_previousPosition + HALF_RACKET_SIZE, PIXEL_OFF);
         // draw new line on racket
         halLcdHLine(xR1, xR1+2, yR1 - HALF_RACKET_SIZE, PIXEL_ON);

         // draw guns if p1 bonus enabled
         if(p1bonusEnabled == 1){
                 // clear old racket1 top gun line
                 halLcdHLine(xR1_old+3, xR1_old+5, yR1_previousPosition - HALF_RACKET_SIZE, PIXEL_OFF);
                 //clear old racket1 bottom gun line
                 halLcdHLine(xR1_old+3, xR1_old+5, yR1_previousPosition + HALF_RACKET_SIZE, PIXEL_OFF);

                 // draw new racket1 top gun line
                 halLcdHLine(xR1+3, xR1+5, yR1 - HALF_RACKET_SIZE, PIXEL_ON);
                 // draw new racket1 bottom gun lines
                 halLcdHLine(xR1+3, xR1+5, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
          }
      }
     // Undraw old racket positions, draw new positions
      if(R1Dir == DOWN){

          //clear old racket1 top line
          halLcdHLine(xR1_old, xR1_old+2, yR1_previousPosition - HALF_RACKET_SIZE, PIXEL_OFF);

          // draw new line on racket
          halLcdHLine(xR1, xR1+2, yR1 + HALF_RACKET_SIZE, PIXEL_ON);

          // draw guns if p1 bonus enabled
          if(p1bonusEnabled == 1){

                 //clear old racket1 top gun line
                 halLcdHLine(xR1_old+3, xR1_old+5, yR1_previousPosition - HALF_RACKET_SIZE, PIXEL_OFF);
                 // clear old racket1 bottom gun line
                 halLcdHLine(xR1_old+3, xR1_old+5, yR1_previousPosition + HALF_RACKET_SIZE, PIXEL_OFF);

                 //draw new racket1 top gun line
                 halLcdHLine(xR1+3, xR1+5, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
                 //draw new racket1 bottom gun lines
                 halLcdHLine(xR1+3, xR1+5, yR1 - HALF_RACKET_SIZE, PIXEL_ON);
          }
      }

      // Undraw old racket positions, draw new positions
      if(R2Dir == UP){
                  //clear old racket2 bottom line
                  halLcdHLine(xR2_old, xR2_old-2, yR2_previousPosition + HALF_RACKET_SIZE, PIXEL_OFF);
                  // draw new line on racket
                  halLcdHLine(xR2, xR2-2, yR2 - HALF_RACKET_SIZE, PIXEL_ON);

                  // if p2bonus is enabled, show player2 inverse racket which is half the size of the normal racket
                  if(p2bonusEnabled){
                      //clear old racket top line
                      halLcdHLine(xR2bonus_old-3, xR2bonus_old-5, yR2bonus_previousPosition - QUARTER_RACKET_SIZE, PIXEL_OFF);
                      // draw new line on racket
                      halLcdHLine(xR2bonus-3, xR2bonus-5, yR2bonus + QUARTER_RACKET_SIZE, PIXEL_ON);
                  }
      }
      // Undraw old racket positions, draw new positions
      if(R2Dir == DOWN){
                  //clear old racket2
                  halLcdHLine(xR2_old, xR2_old-2, yR2_previousPosition - HALF_RACKET_SIZE, PIXEL_OFF);
                  // draw new line on racket
                  halLcdHLine(xR2, xR2-2, yR2 + HALF_RACKET_SIZE, PIXEL_ON);

                  // if p2bonus is enabled, show player2 inverse racket which is half the size of the normal racket
                  if(p2bonusEnabled){
                      //clear old bonusracket2 bottom line
                      halLcdHLine(xR2bonus_old-3, xR2bonus_old-5, yR2bonus_previousPosition + QUARTER_RACKET_SIZE, PIXEL_OFF);
                      // draw new line on racket
                      halLcdHLine(xR2bonus-3, xR2bonus-5, yR2bonus - QUARTER_RACKET_SIZE, PIXEL_ON);
                  }
      }

      // Update old position values
      xR1_old = xR1;
      yR1_old = yR1;
      xR2_old = xR2;
      yR2_old = yR2;

      // Update old position values for bonus racket
      if(p2bonusEnabled){
           xR2bonus_old = xR2bonus;
           yR2bonus_old = yR2bonus;
      }

      // Mark new positions are having been drawn
      newPositionsDrawn = 1;
      newAIPositionsDrawn = 1;
}

/*********************************************************
 ********** Game States initializations ***********************
 *********************************************************/

// Changes game state to LOADING
void GameLoadInit(){
    gameStateInstance = LOADING;
}

// Resets variable values, sets game state to INTRO and bonus state to OUTOFPLAY
void GameIntroInit(){

    gameStateInstance = INTRO; // Want to set this here so that during the game_update() function it will show menu for play mode, e.g. 2player or 1 player vs AI, before play begins
    toggle_AI_direction = +1;
    newPositionsDrawn = 0;
    newAIPositionsDrawn = 0;
    p1Score = 0;
    p1bonusEnabled = 0;
    p1bonusCooldown = 0;
    p2Score = 0;
    p2bonusEnabled = 0;
    p2bonusCooldown = 0;
    BonusStatus = OUTOFPLAY;
    LastHitterInstance = P1;
    // Set the winning score value
    winningScore = 5;
}

/* Set variable initial values,
 * Set initial racket positions and directions
 * Set bonus-related initial values
 * Clear inputupdate and lcdupdate pending vars
 * Change game state to STARTING
 */
void GameStartInit()
{
     InputUpdatePending = 0;
     LCDUpdatePending = 0;

     // draw top wall
     halLcdHLine(0,LCD_COL,12,PIXEL_ON);

     //Initial position of racket 1
     xR1 = 0; //left-hand side
     yR1 = LCD_ROW >> 1; //middle row
     xR1_old = xR1;
     yR1_old = yR1;

     //Initial position of racket 2
     xR2 = LCD_COL - 1; //right-hand side
     yR2 = LCD_ROW >> 1; //middle row
     xR2_old = xR2;
     yR2_old = yR2;

     //Initial position of bonus racket 2
     xR2bonus = LCD_COL -1;
     yR2bonus = LCD_ROW >> 1; //middle row
     xR2bonus_old = xR2bonus;
     yR2bonus_old = yR2bonus;

     //Draw new racket1
     halLcdVLine(xR1, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
     halLcdVLine(xR1 + 1, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);
     //halLcdVLine(xR1 + 2, yR1 - HALF_RACKET_SIZE, yR1 + HALF_RACKET_SIZE, PIXEL_ON);

     //Draw new racket2
     halLcdVLine(xR2, yR2 - HALF_RACKET_SIZE, yR2 + HALF_RACKET_SIZE, PIXEL_ON);
     halLcdVLine(xR2 - 1, yR2 - HALF_RACKET_SIZE, yR2 + HALF_RACKET_SIZE, PIXEL_ON);
     //halLcdVLine(xR2 - 2, yR2 - HALF_RACKET_SIZE, yR2 + HALF_RACKET_SIZE, PIXEL_ON);

     yR1_old = yR1;
     //yR1_previousPosition = yR1;
     xR1_old = xR1;
     yR2_old = yR2;
     //yR2_previousPosition = yR2;
     xR2_old = xR2;

     R1Dir = STOP;
     R2Dir = STOP;

     //Bonus initialisation
     bonusScore = 1;
     bonusDrawn = 0;
     p1Projectiles_onscreen = 0;
     p1Projectiles_active = 0;
     p1_projectileA_x_displacement = +2; // move to the right
     p1_projectileB_x_displacement = +2; // move to the right

     //Initial state of the ball
     gameStateInstance = STARTING;
}
