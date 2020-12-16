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
//
//  Additional features added by mk December 2020
//******************************************************************************	

#include "pong_auxiliary.h"

/* 5xx functions / variables */ 
volatile unsigned int LCD_intervals = 0; //count number of base intervals elapsed
//volatile =  compiler will not optimize these variables

//main function
void main(void)
{
  
  WDTCTL = WDTPW+WDTHOLD; // Stop WDT
  
  // Initialize board
  halBoardInit();  

  /* Initialize TimerA1, it times LCD updates */
  TimerA1Init();

  //Initialize LCD and backlight
  LCDInit();

  //Initialize game variables
  GameStartInit();

  // Initialise Game Intro variables
  GameIntroInit();

  // Set game state to starting up
  GameLoadInit();

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

    if(LCDUpdatePending) // If TimerA has timed out 9 times
    {
        if(InputUpdatePending || ContinuousPressChecker) // If user input has been pressed this time, or last time
        {
            InputUpdatePending=0; // clear var
            ContinuousPressChecker = 0; // clear var
            UserInputs_update(); // update positions if inputs are currently pressed, also set ContinuousPressChecker if they are
        }

        if(AI_enabled == 1){ // If vs AI was chosen at start of the game
            ai_movement(); // update positions for AI
        }

        game_update(); // Based on the current game state perform game operations

        LCDUpdatePending=0; // Clear var
        LCD_update(); // Undraw old object positions, draw new object positions
    }

  }   	  
}

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
   LCD_intervals++; // incrementer
   if(LCD_intervals>=(LCD_INTERVAL_mS/TIMING_BASE_mS)) //8 is value being checked against
   {
      LCDUpdatePending = 1; //warn the CPU that LCD update is required
      LCD_intervals = 0;
   }
 }

 if(LCDUpdatePending)
 {
     //keep CPU active after ISR to process updates in main loop
   __bic_SR_register_on_exit(LPM3_bits);
 }
}

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

        InputUpdatePending  = 0; // clear var
    }

    if(gameStateInstance == WINNING){

            // Clear LCD
            LCDInit();

            // Restart TimerA
            TimerA1Init();

            // Change game state to starting
            GameStartInit();

            // Reset vars such as scores and show Intro Menu
            GameIntroInit();

        }

    if(gameStateInstance == LOADING){

            if((!(P2IN & BIT4))){ //JOYSTICK UP pressed


                // Restart TimerA
                TimerA1Init();

                // Clear LCD
                LCDInit();

                // Reset vars such as scores and show Intro Menu
                GameIntroInit();

            }
    }

    /* Don't keep CPU awake to process the input, instead wait for TimerA interrupt to wake CPU
     * __bic_SR_register_on_exit(LPM3_bits);
     */
}



