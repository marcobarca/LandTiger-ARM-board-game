#include "button.h"
#include "lpc17xx.h"
#include "../RIT/RIT.h"		  			
#include "../game.h"
#include "../timer.h"
int debugEINT0 = 0;
int debugEINT1 = 0;
int debugEINT2 = 0;
extern int direction_led;
extern bool direction_led_flag;

int button1 = 0;
int	button2 = 0;

void EINT0_IRQHandler (void)	  	/* ------ START BUTTON ------	*/
{		
	start();
	direction_led_flag = true;
 
	NVIC_DisableIRQ(EINT0_IRQn);						 //Disable interrupt 0
	LPC_PINCON->PINSEL4    |= (1 << 22);     //Enable button 1 
	LPC_PINCON->PINSEL4    |= (1 << 24);     //Enable button 2 
	LPC_PINCON->PINSEL4    &= ~(1 << 20);    //Disable button 0						
	LPC_SC->EXTINT &= (1 << 0);    					 // clear pending interrupt          
}


void EINT1_IRQHandler (void)	  	/* ------ ROTATE BUTTON ------ */
{
	button1=1;
	enable_RIT();										           // enable RIT to count 50ms				 
	NVIC_DisableIRQ(EINT1_IRQn);		           // disable Button interrupts			   
	LPC_PINCON->PINSEL4    &= ~(1 << 22);      // GPIO pin selection               
	LPC_SC->EXTINT &= (1 << 1);                // clear pending interrupt          
}

void EINT2_IRQHandler (void)	  	/* -------- RUN BUTTON ------- */
{
	button2=1;
	enable_RIT();													    // enable RIT to count 1s				      
	NVIC_DisableIRQ(EINT2_IRQn);							// disable Button interrupts			    
	LPC_PINCON->PINSEL4    &= ~(1 << 24);     // GPIO pin selection                 
  LPC_SC->EXTINT &= (1 << 2);     					// clear pending interrupt              
}


