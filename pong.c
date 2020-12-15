//******************************************************************************
//  Pong.c
//
//  Description: Starting point for the Pong game coursework
//  Core functionality is completed (timed operation)
//  ACLK = REFO = LFXT1 32kHz, MCLK = SMCLK = 1MHz
//
//  M. MATA
//  GCU
//  November 2017
//******************************************************************************	

#include <game_mechanics.h>
#include    "msp430f5438a.h"
#include	"hal_UCS.h"
#include 	"hal_PMM.h"
#include    "hal_lcd.h"
#include    "general_settings.h"


/* 5xx functions / variables */ 
void ai_movement(void);
void halBoardInit(void);
void LCDInit(void);
void TimerB0Init(void);
void TimerA1Init(void);
void GameIntroInit(void);
void GameStartInit(void);
void UserInputs_update(void);
void LCD_update(void);


volatile unsigned int LCD_intervals = 0; //count number of base intervals elapsed
volatile unsigned int Game_intervals = 0; //count number of base intervals elapsed
//volatile =  compiler will not optimize these variables


//main function
void main(void)
{
  volatile unsigned long  batt_voltage; 
  
  WDTCTL = WDTPW+WDTHOLD; // Stop WDT
  
  // Initialize board
  halBoardInit();  

  /* Initialize TimerB0, it provides a periodic timer for checking for conintuous input pressing */
  //TimerB0Init();

  /* Initialize TimerA1, it times ball and LCD updates */
  TimerA1Init();

  /* Initialize LFXT1, it will drive TA1 and TB0 */
  //P7SEL |= BIT1+BIT0;    // Enable crystal pins
  //LFXT_Start(XT1DRIVE_0);

  //Initialize LCD and backlight
  LCDInit();

  //Initialize game variables
  GameStartInit();

  // Initialise Game Intro variables
  GameIntroInit();


  while(1) //infinite main loop
  {
    // Bit-set (LPM3_bits + GIE) in SR register to enter LPM3 mode    
    __bis_SR_register(LPM3_bits + GIE); 
    __no_operation(); //for debug

    //CPU CONTINUES HERE WHEN IT IS AWAKEN AT THE END OF TimerA1 ISR

    /* If input vector has been triggered then that ISR sets InputUpdatePending,
     *
     * Also want to update positions if an input is being continuously pressed.
     * The input interrupt will not register if an input is continuously pressed,
     * only the first time it was pressed, due to triggering on an edge.
     * So this needs to be handled, and it is done in code by setting a var to check
     * for continuous pressing after an input interrupt has happened. The var is set
     * in the UserInputs_update function, e.g. if input currently high, then check it
     * again next time round
     */

    if(InputUpdatePending || ContinuousPressChecker)
    {
                 InputUpdatePending=0; // clear var
                 ContinuousPressChecker = 0; // clear var
                 UserInputs_update(); // update positions if inputs are currently pressed, also set ContinuousPressChecker if they are
    }


    if(GameUpdatePending)
    {
        GameUpdatePending=0;
     game_update();
    }



    if(LCDUpdatePending)
    {
        if(AI_enabled == 1)
            ai_movement(); // update positions if inputs are currently pressed, also set ContinuousPressChecker if they are
     LCDUpdatePending=0;
     LCD_update();
    }

  }   	  
}



//LCD initialization
void LCDInit(void)
{
 halLcdInit();
 halLcdBackLightInit();
 halLcdSetBackLight(LCD_BACKLIGHT_LEVEL);
 halLcdSetContrast(LCD_CONTRAST_LEVEL);
 halLcdClearScreen();
}

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

//This function can be used to initialize game variables at boot-up
//Most variables are  declared into general_settings.h
void GameStartInit()
{
     InputUpdatePending = 0;
     GameUpdatePending = 0;
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

void ai_movement(void){
    if(newAIPositionsDrawn ==1){
        newAIPositionsDrawn = 0;

        if( (xBall < (LCD_COL - 6)) && (yR2 > (HALF_RACKET_SIZE+13)) && (yR2 > yBall) ){ // if AI racket not hitting top wall, and also lower than the ball, move racket up, unless ball is close to racket
            yR2_previousPosition = yR2;
            //yR2_old = yR2;
            yR2 -= 1; //move racket 1 pixels up
            R2Dir = UP;

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
        if( (xBall < (LCD_COL - 6)) && (yR2 < (LCD_ROW - HALF_RACKET_SIZE)) && (yR2 < yBall) ){ // if AI racket not hitting bottom wall, and also higher than ball, move racket down, unless ball is close to racket
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
        // 'human movement' if ball is close to the racket
        if(xBall > (LCD_COL - 6) && (yR2 > (HALF_RACKET_SIZE+13)) ){
            if(toggle_AI_direction > 0){
                yR2_previousPosition = yR2;
                //yR2_old = yR2;
                yR2 -= 1; //move racket 1 pixels up
                R2Dir = UP;
                return;
            }
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
}

//Read user inputs here (CPU is awaken by ADC12 conversion)
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

//Update drawings in LCD screen (CPU is awaken by TimerA1 interval ints)
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
         //halLcdPrintXY("B",BONUS_APPEAR_X, BONUS_APPEAR_Y,OVERWRITE_TEXT);//draw bonus
         bonusDrawn = 1;
     }
     // If bonus has been drawn and now has been hit by a player, it needs to be undrawn
     if( (BonusStatus == PICKEDUP ) && (bonusDrawn == 1) ){
         halLcdCircle( BONUS_APPEAR_X, BONUS_APPEAR_Y, BONUS_RADIUS, PIXEL_OFF);//undraw bonus
         //halLcdPrintXY("B",BONUS_APPEAR_X, BONUS_APPEAR_Y,INVERT_TEXT);//undraw bonus
         bonusDrawn = 0; // clear the bonus drawn variable
     }

     if( (p1bonusEnabled == 1) && (p1Projectiles_onscreen == 1)  ){

         // Clear old projectiles

         halLcdHLine(p1ProjectileA_X_old2 - PROJECTILE_HALF_SIZE, p1ProjectileA_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old2, PIXEL_OFF);
         halLcdHLine(p1ProjectileB_X_old2 -PROJECTILE_HALF_SIZE, p1ProjectileB_X_old2 + PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old2, PIXEL_OFF);
                  // draw new line on racket

         /*
         halLcdCircle( p1ProjectileA_X_old2, p1ProjectileA_Y_old2, PROJECTILE_RADIUS, PIXEL_OFF);
         halLcdCircle( p1ProjectileB_X_old, p1ProjectileB_Y_old2, PROJECTILE_RADIUS, PIXEL_OFF);
*/

         // Draw projectile trails
         halLcdHLine(p1ProjectileA_X_old-PROJECTILE_HALF_SIZE, p1ProjectileA_X_old+PROJECTILE_HALF_SIZE, p1ProjectileA_Y_old, PIXEL_ON);
         halLcdHLine(p1ProjectileB_X_old-PROJECTILE_HALF_SIZE, p1ProjectileB_X_old +PROJECTILE_HALF_SIZE, p1ProjectileB_Y_old, PIXEL_ON);


        /* halLcdCircle( p1ProjectileA_X_old, p1ProjectileA_Y_old, PROJECTILE_RADIUS, PIXEL_ON);
         halLcdCircle( p1ProjectileB_X_old, p1ProjectileB_Y_old, PROJECTILE_RADIUS, PIXEL_ON);
*/

         // Draw new projectile positions
         halLcdHLine(p1ProjectileA_X-PROJECTILE_HALF_SIZE, p1ProjectileA_X+PROJECTILE_HALF_SIZE, p1ProjectileA_Y , PIXEL_ON);
         halLcdHLine(p1ProjectileB_X-PROJECTILE_HALF_SIZE, p1ProjectileB_X+PROJECTILE_HALF_SIZE, p1ProjectileB_Y, PIXEL_ON);

         /*
         halLcdCircle( p1ProjectileA_X, p1ProjectileA_Y, PROJECTILE_RADIUS, PIXEL_ON);
         halLcdCircle( p1ProjectileB_X, p1ProjectileB_Y, PROJECTILE_RADIUS, PIXEL_ON);
         */

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

          xR1_old = xR1;
          yR1_old = yR1;
          xR2_old = xR2;
          yR2_old = yR2;
          if(p2bonusEnabled){
              xR2bonus_old = xR2bonus;
              yR2bonus_old = yR2bonus;
          }

          newPositionsDrawn = 1;
          newAIPositionsDrawn = 1;
}

/*********************************************************
 ********** System initializations ***********************
 *********************************************************/
//MSP430 pins initialization
void halBoardInit(void)
{
  // Tie unused ports and pins as LOW outputs to save power
    //PA = P1 and P2 (16 bits)
    //PB = P3 and P4 (16 bits)
    //PC = P5 and P6 (16 bits)
    //PD = P7 and P8 (16 bits)
    //PE = P9 and P10 (16 bits)
  PAOUT  = 0; PADIR  = 0xFFFF; PASEL  = 0;
  PBOUT  = 0; PBDIR  = 0xFFFF; PBSEL  = 0;
  PCOUT  = 0; PCDIR  = 0xFFFF; PCSEL  = 0;
  PDOUT  = 0; PDDIR  = 0xFFFF; PDSEL  = 0;
  // P10.0 to USB RST pin, if enabled with J5
  PEOUT  = 0; PEDIR  = 0xFEFF; PESEL  = 0;
  P11OUT = 0; P11DIR = 0xFF;   P11SEL = 0;
  PJOUT  = 0; PJDIR  = 0xFF;

  P6OUT = 0x40;      // Shut down audio output amp
  P5DIR &= ~0x80;    // USB RX Pin, Input with pull-down
  P5OUT &= ~0x80;
  P5REN |= 0x80;

  // LED outputs
  P1DIR = P1DIR | (BIT0 + BIT1);

  //Now configure SW1 (P2.6) and SW2 (P2.7) as inputs for Player2 with pull-down (example)
  P2DIR &= ~(BIT6+BIT7); //pin 6 input
  P2REN = P2REN | (BIT6+BIT7); //pin 6 internal pull R enabled
  P2OUT = P2OUT | (BIT6+BIT7); //pin 6 pull-down

  //configure JOYSTICK DOWN (P2.5) and UP (P2.4) and right (P2.2)
  P2SEL = P2SEL & ~(BIT5 + BIT4 + BIT2); //GPIO
  P2DIR = P2DIR & ~(BIT5 + BIT4 + BIT2); //input
  P2REN = P2REN | (BIT5 + BIT4 + BIT2);  //internal resistor enabled
  P2OUT = P2OUT | (BIT5 + BIT4 + BIT2);  //pull up, so pressing it makes it LOW

  //configure interrupts for mechanical inputs
  // Set-up the interrupt
   P2IES |= ( BIT7 + BIT6 + BIT5 + BIT4 +BIT2 ); // High/Low edge (falling edge)
   P2IFG &= ~( BIT7 + BIT6 + BIT5 + BIT4 +BIT2 ); //IFG bits cleared
   P2IE |= ( BIT7 + BIT6 + BIT5 + BIT4 + BIT2); //Enable pins interrupts
}

//NOTE: TimerA0 is initialized inside hal_lcd.c because it's already used for the LCD backlight PWM
//Initialize TimerA1 as basic intervals generator
void TimerA1Init(void)
{
  //TA1CCR0   = TIMING_BASE_mS*(Faclk/1000);
  // TIMING_BASE_mS*(32.768) ~ TIMING_BASE_mS*(33)
  TA1CCR0   = TIMING_BASE_mS*33;
  TA1CCTL0 = CCIE; //Enable TA1CCR0 CCIFG int
  TA1CTL    = TASSEL_1 + MC_1 + TACLR; // ACLK, UP mode. Do not enable TAIE (rollover int)
}

/*
//Initialize TimerB0 to drive the ADC12 and inputs reading
void TimerB0Init(void)
{
  //TB0CCR0   = INPUT_INTERVAL_mS*(Faclk/1000);
  // INPUT_INTERVAL_mS*(32.768) ~ INPUT_INTERVAL_mS*(33)
  TB0CCR0   = INPUT_INTERVAL_mS*33; //set timer interval
  // Provide CCR1 value, time until CCR1 match must be greater than 75 us!
  TB0CCR1   = 3;         //this time will be 3 * 1/32768kHZ (> 75usec)
  TB0CCTL1  = OUTMOD_3;  // CCR1 output mode => SET/RESET. Do not enable TB0CCR3 INT
  // ACLK, UP mode, clear TBR, enable rollover INT
  TB0CTL    = TBSSEL_1 + MC_1 + TBCLR + TBIE;
}
*/

/***********************************************************************
 *************** Interrupt Service Routines (ISRs) *********************
 ***********************************************************************/
// Timer A1 CCR0 interrupt service routine
#pragma vector=TIMER1_A0_VECTOR
__interrupt void TIMER1_A0_ISR(void)
{
 //interrupt flag CCIFG is automatically cleared when servicing this ISR
  if(!LCDUpdatePending) //if update still pending, skip this interval
 {
   LCD_intervals++;
   if(LCD_intervals>=(LCD_INTERVAL_mS/TIMING_BASE_mS)) //time to run the ball update
   {
      LCDUpdatePending = 1; //warn the CPU that LCD update is required
      LCD_intervals = 0;
   }
 }



 if(!GameUpdatePending) //if update still pending, skip this interval
 {
   Game_intervals++;
   if(Game_intervals>=(GAME_INTERVAL_mS/TIMING_BASE_mS)) //time to run the ball update
   {
     GameUpdatePending = 1; //warn the CPU that ball update is required
     Game_intervals = 0;
   }
 }

 if(GameUpdatePending || LCDUpdatePending)
 {
     //keep CPU active after ISR to process updates in main loop
   __bic_SR_register_on_exit(LPM3_bits);
 }
}
/*
#pragma vector=TIMER0_B1_VECTOR
__interrupt void TIMER0_B1_ISR(void)
{
  switch(__even_in_range(TB0IV,14))
  {
    case  0: break;                // No interrupt
    case  2: break;                // TB0CCR1 not used
    case  4: break;                // TB0CCR2 not used
    case  6: break;                // TB0CCR3 not used
    case  8: break;                // TB0CCR4 not used
    case 10: break;                // TB0CCR5 not used
    case 12: break;                // TB0CCR6 not used
    case 14:                       // TBIFG overflow, enable new ADC
        InterruptTimeoutPending = 1; //warn the CPU that input update is required
            //Keep CPU active on exit to process user inputs in main loop
            __bic_SR_register_on_exit(LPM3_bits);
            break;
  }
}
*/

// Port 2 Interrupt Service Routine (ISR)
#pragma vector=PORT2_VECTOR  //attaches function my_Port2_ISR to P2 interrupt
__interrupt void my_Port2_ISR(void)
{

    InputUpdatePending = 1; //warn the CPU that input update is required
    if((P2IFG & BIT5)) //JOYSTICK DOWN pressed
           P2IFG &= ~BIT5;    // interrupt serviced, clear corresponding interrupt flag

    if((P2IFG & BIT4)) //JOYSTICK UP pressed
           P2IFG &= ~BIT4;    // interrupt serviced, clear corresponding interrupt flag

    if((P2IFG & BIT2)) //JOYSTICK RIGHT pressed
               P2IFG &= ~BIT2;    // interrupt serviced, clear corresponding interrupt flag

    if((P2IFG & BIT6)) //SW1 pressed for UP
            P2IFG &= ~BIT6; // interrupt serviced, clear corresponding interrupt flag

    if((P2IFG & BIT7)) //SW2 pressed for DOWN
                P2IFG &= ~BIT7; // interrupt serviced, clear corresponding interrupt flag

    if(gameStateInstance == INTRO){
        if((!(P2IN & BIT4))) //JOYSTICK UP pressed
            AI_enabled = 0;
        if((!(P2IN & BIT5))) // JOYSTICK DOWN pressed
            AI_enabled = 1;

        // Initialise and start gameplay
        // Restart TimerA
        TimerA1Init();

        // Clear LCD
        LCDInit();

        // Change game state to starting
        GameStartInit();

        InputUpdatePending  = 0;
    }

    if(gameStateInstance == SCORING){
        // Restart TimerA
        TimerA1Init();

        // Clear LCD
        LCDInit();

        // Change game state to starting
        GameStartInit();
    }

    if(gameStateInstance == WINNING){
            // Restart TimerA
            TimerA1Init();

            // Clear LCD
            LCDInit();

            // Change game state to starting
            GameStartInit();

            // Reset scores and show Intro Menu
            GameIntroInit();

        }


    /* Don't keep CPU awake to process the input, instead wait for TimerA interrupt to wake CPU
     * __bic_SR_register_on_exit(LPM3_bits);
     */
}

