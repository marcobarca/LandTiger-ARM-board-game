#include <stdio.h>
#include "LPC17xx.H"                    /* LPC17xx definitions                */
#include "timer.h"
#include "RIT/RIT.h"
#include ".\GLCD\GLCD.h"
#include ".\TouchPanel\TouchPanel.h"
#include ".\joystick\joystick.h"
#include ".\game.h"

#ifdef SIMULATOR
extern uint8_t ScaleFlag; 
#endif
/*----------------------------------------------------------------------------
  Main Program
 *----------------------------------------------------------------------------*/
int main (void) {
  	
	SystemInit();  												/* System Initialization (i.e., PLL)  */
  LCD_Initialization();
	TP_Init();
	TouchPanel_Calibrate();
	LCD_Clear(White);
	draw_GUI();
	joystick_init();

	init_RIT(0x004C4B40);									
	init_timer(0,0x004C4B40);							
	enable_timer(0);
	
	LPC_SC->PCON |= 0x1;									/* power-down	mode										*/
	LPC_SC->PCON &= ~(0x2);						
		
  while (1) {                           /* Loop forever                       */	
		__ASM("wfi");
  }

}
