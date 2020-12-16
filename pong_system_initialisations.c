


#include "pong_systems_initialisations.h"

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
  //P1DIR = P1DIR | (BIT0 + BIT1);

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
//LCD initialization
void LCDInit(void)
{
 halLcdInit();
 halLcdBackLightInit();
 halLcdSetBackLight(LCD_BACKLIGHT_LEVEL);
 halLcdSetContrast(LCD_CONTRAST_LEVEL);
 halLcdClearScreen();
}




