
#include "game_mechanics_auxiliary.h"

/*
 * Causes the ball displacement to change depending on the which racket zone hit for a stationary racket
 * or depending on which direction the racket is moving for a moving racket
 * */
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

// Undraw projectiles old positions
void clearProjectiles(){

    // Clear old projectiles
    halLcdHLine(p1ProjectileA_X - PROJECTILE_HALF_SIZE, p1ProjectileA_X + PROJECTILE_HALF_SIZE, p1ProjectileA_Y, PIXEL_OFF);
    halLcdHLine(p1ProjectileB_X -PROJECTILE_HALF_SIZE, p1ProjectileB_X + PROJECTILE_HALF_SIZE, p1ProjectileB_Y, PIXEL_OFF);
    halLcdHLine(p1ProjectileA_X_old - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old, PIXEL_OFF);
    halLcdHLine(p1ProjectileB_X_old -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old, PIXEL_OFF);
    halLcdHLine(p1ProjectileA_X_old2 - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old2, PIXEL_OFF);
    halLcdHLine(p1ProjectileB_X_old2 -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old2, PIXEL_OFF);
}

/*
 * Puts projectiles off-screen in terms of x position
 * Sets the projectile y position which corresponds with a racket2 cannon y position
 * */
void resetProjectiles(){

    // Set projectile A position off screen for now so as to not interfere with the ball
    p1ProjectileA_X = xR1-10;
    p1ProjectileA_Y = yR1 - HALF_RACKET_SIZE; // Update the y-position to be at a racket2 cannon

    // Update previous/old for projectile A
    p1ProjectileA_X_old = p1ProjectileA_X;
    p1ProjectileA_X_old2 = p1ProjectileA_X;
    p1ProjectileA_Y_old = p1ProjectileA_Y;
    p1ProjectileA_Y_old2 = p1ProjectileA_Y;


    // Set projectile B position off screen for now so as to not interfere with the ball
    p1ProjectileB_X = xR1-10;
    p1ProjectileB_Y = yR1 + HALF_RACKET_SIZE; // Update the y-position to be at a racket2 cannon

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

// Prevent ball from passing through ceiling
// also return value to indicate whether ceiling reached by ball
int top_wall_reached()
{
    if(yBall <= (BALL_RADIUS+12) ) //top wall reached
    {
        yBall = 1+BALL_RADIUS+12; //do not overwrite the wall
        return 1;
    }
    else return 0;
}

// Prevent ball from passing through floor
// also return value to indicate whether floor reached by ball
int bottom_wall_reached()
{
    if(yBall >= (LCD_ROW-BALL_RADIUS)) //up wall reached
    {
        yBall = LCD_ROW-BALL_RADIUS; //do not overwrite the wall
        return 1;
    }
    else return 0;
}

// Return value to indicate whether right wall reached
int right_wall_reached()
{
    if(xBall >= LCD_COL-1-BALL_RADIUS) //right wall reached
    {
        //xBall = LCD_COL-2-BALL_RADIUS; //do not overwrite right wall/racket
        return 1;
    }
    else return 0; //right wall not reached
}

// Return value to indicate whether left wall reached
int left_wall_reached()
{
    if(xBall <= BALL_RADIUS)  //left wall reached
    {
        //xBall = BALL_RADIUS; //do not overwrite wall/racket1
        return 1;
    }
    else return 0; //right wall not reached
}

// Return value to indicate whether Player1 racket hit by ball
int P1_racket_hit() //check ball vs left racket
{
 if( (yBall <= (yR1 + HALF_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}

// Return value to indicate whether Player1 racket centre hit by ball
int P1_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR1 + QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - QUARTER_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}
// Return value to indicate whether Player1 racket upper hit by ball
int P1_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR1 - QUARTER_RACKET_SIZE)) && (yBall >= (yR1 - HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}
// Return value to indicate whether Player1 racket lower hit by ball
int P1_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR1 + QUARTER_RACKET_SIZE)) && (yBall <= (yR1 + HALF_RACKET_SIZE)) && (xBall <= BALL_RADIUS+4) )
     return 1;
 else
     return 0;
}

/*
 * Return value to indicate whether Player2 bonus racket hit by ball,
*also adjust ball displacement depending on bonus racket movement
 */
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
// Return value to indicate whether Player2 racket hit by ball
int P2_racket_hit() //check ball vs right racket
{
 if( (yBall <= (yR2 + HALF_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= (LCD_COL - BALL_RADIUS - 4)) )
     return 1;
 else
     return 0;
}
// Return value to indicate whether Player2 racket centre hit by ball
int P2_racket_centre_hit() //check ball vs left racket centre
{
 if( (yBall <= (yR2 + QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - QUARTER_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
     return 1;
 else
     return 0;
}
// Return value to indicate whether Player2 racket upper hit by ball
int P2_racket_upper_hit() //check ball vs left racket upper
{
 if( (yBall < (yR2 - QUARTER_RACKET_SIZE)) && (yBall >= (yR2 - HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
     return 1;
 else
     return 0;
}
// Return value to indicate whether Player2 racket lower hit by ball
int P2_racket_lower_hit() //check ball vs left racket lower
{
 if( (yBall > (yR2 + QUARTER_RACKET_SIZE)) && (yBall <= (yR2 + HALF_RACKET_SIZE)) && (xBall >= BALL_RADIUS-4) )
     return 1;
 else
     return 0;
}

// updates the top banner string which holds e.g. "B P1:X vs P2:X B"
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

