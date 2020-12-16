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


//Update the state and position of the ball
//(CPU is awaken by TimerA1 interval ints)
void game_update(void)
{
    switch(BonusStatus){

        case INPLAY:
            bonus_enable();
            break;

        case PICKEDUP:
            handle_bonus_projectile_hitting_ball();
            break;

        case OUTOFPLAY:

    }

 //calculate new position and bouncing
 switch(gameStateInstance)
 {
  case LOADING:
     game_load();
     break;

  case INTRO: // Game intro is happening, e.g. menu selection
     game_intro();
     break;

  case STARTING: //"Start" state, init ball position
     game_start();
     break;

  case MOVING: //moving objects in free space
     moving();
     break;

  case SCORING:

     //Handle scoring and winning
     switch(Scorer){
          case PLAYER1:
              player1scoring();
              break;

          case PLAYER2:
              player2scoring();
              break;
     }
     break;

  case WINNING:
     winning();
     break;
 } // end of switch gameStateInstance

}// end of game_update


void bonus_enable(void){

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
}


void handle_bonus_projectile_hitting_ball(void){

    // If the ball meets the projectile, destroy projectile, then flip the ball x and y directions
            if(
                  (       // ball x coordinate is within 2*projectileLength of projectileA x value
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


                x_displacement = (~x_displacement)+1;
                y_displacement = (~y_displacement)+1;
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

                x_displacement = (~x_displacement)+1;
                y_displacement = (~y_displacement)+1;
            }
}

void game_load(void){

          halLcdClearScreen(); //CLEAR SCREEN

          halLcdPrintLine("Welcome Back To:", 1, OVERWRITE_TEXT);//PRINT MESSAGE ......... NB ADD SCORES TO THE SCREEN
          halLcdPrintLine("The Days Of...:", 2, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("  -----------  ", 3, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("   P     N   ", 4, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("      O     G ", 5, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("  ------------  ", 6, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("Move JOYSTICK UP", 7, OVERWRITE_TEXT);//PRINT MESSAGE

          //stop TimerA1. This prevents new LCD and ball updates
          //but user input is operational as Port2 interrupts can still be triggered
          TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111
}

void game_intro(void){

          halLcdClearScreen(); //CLEAR SCREEN

          halLcdPrintLine("Choose play mode:", 1, OVERWRITE_TEXT);//PRINT MESSAGE ......... NB ADD SCORES TO THE SCREEN
          halLcdPrintLine("JOYSTICK UP", 3, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("==2 player==", 4, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("JOYSTICK DOWN", 6, OVERWRITE_TEXT);//PRINT MESSAGE
          halLcdPrintLine("==vs AI==", 7, OVERWRITE_TEXT);//PRINT MESSAGE

          p1Score = 0;
          p2Score = 0;

          //stop TimerA1. This prevents new LCD and ball updates
          //but user input is operational as Port2 interrupts can still be triggered
          TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111
}

void game_start(void){
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

              if( (yR1 == (LCD_ROW >> 1)) && (yR2 == (LCD_ROW >> 1)) ) // if rackets in the correct starting position, proceed to next game state
                  gameStateInstance = MOVING; // begin moving
}

void moving(void){
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

        // Update the ball position by the displacement amount
        xBall = xBall + x_displacement;
        yBall = yBall + y_displacement;

        if(top_wall_reached()){
            y_displacement = (~y_displacement)+1; // flip the y direction of the ball
        }

        if(bottom_wall_reached()){
            y_displacement = (~y_displacement)+1; // flip the y direction of the ball
        }

        if(P1_racket_hit()){
            LastHitterInstance = P1;
            x_displacement = +1; // 'bounce' the ball off the racket and keep it moving toward opposite side
            racket_movement_effect(1); // check to see if racket is moving or not, and add to the ball's direction displacement depending on this
            if(AI_enabled) // if AI is playing, change it's hitting movement direction
               toggle_AI_direction  = (~toggle_AI_direction)+1;
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
            Scorer =  PLAYER2;
            p2Score += 1; // Increment player2 score
            updateScoreString(scoreString,2); // Update the score string with the latest player who scored
            gameStateInstance = SCORING;
            x_displacement = -1; // When play restarts ball will go toward P2
        }
        if(right_wall_reached()){
            Scorer =  PLAYER1;
            p1Score += 1; // Increment player1 score
            updateScoreString(scoreString,1); // Update the score string with the latest player who scored
            gameStateInstance = SCORING;
            x_displacement = +1; // When play restarts ball will go toward P1
        }
}

void player1scoring(void){
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

       updateBannerString(bannerString,p1Score,p2Score,p1bonusEnabled,p2bonusEnabled);
       halLcdPrintLine(bannerString, 0, OVERWRITE_TEXT);//PRINT MESSAGE

       updateScoreString(scoreString,1);
       halLcdPrintXY( scoreString, 10, 80, OVERWRITE_TEXT); // Print GOAL <player> while game paused

       gameStateInstance = STARTING;
   }
}
void player2scoring(void){
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
        halLcdPrintXY( scoreString, 10, 80, OVERWRITE_TEXT);// Print GOAL <player> while game paused

        gameStateInstance = STARTING;
    }
}

void winning(void){
    halLcdPrintLine("   GAME OVER    ", 0, OVERWRITE_TEXT);//Overwrite banner with 'GAME OVER'
    updateCurrentScoresString(currentScoresString,p1Score,p2Score); // Update the current scores string with current player scores
    halLcdPrintLine(currentScoresString,4,OVERWRITE_TEXT);  // print to LCD
    halLcdPrintLine(" Press an input   or Reset btn", 5, OVERWRITE_TEXT);//PRINT MESSAGE
    halLcdPrintLine(" to continue", 7, OVERWRITE_TEXT);//PRINT MESSAGE

    //stop TimerA1. This prevents new LCD and ball updates
    //but user input is operational as Port2 interrupts can still be triggered
    TA1CTL= TA1CTL & ~(BIT5 + BIT4); //MC=00 (bits 5,4) 0b11001111
}
