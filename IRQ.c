#include "lpc17xx.h"
#include "timer.h"
#include "led/led.h"
#include "game.h"
#include "RIT/RIT.h"

int mod=2;


void RIT_IRQHandler (void)
{					
	static int select=0;
	
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0){	
		/* Joytick Select pressed */
		select++;
		switch(select){
			case 1:
				if(mod==1)
					mod=2;
				else
					mod=1;
				break;
			default:
				break;
		}
	}
	else{
			select=0;
	}
	
  LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
	
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
