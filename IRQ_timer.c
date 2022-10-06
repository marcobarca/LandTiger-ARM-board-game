#include "lpc17xx.h"
#include "timer.h"
#include "game.h"
#include "RIT/RIT.h"
#include ".\TouchPanel\TouchPanel.h"
#include ".\GLCD\GLCD.h"

extern bool you_win_flag;
static bool started = false;

void TIMER0_IRQHandler (void){
	reset_timer(0);
	
  getDisplayPoint(&display, Read_Ads7846(), &matrix ) ;
	//Touch the screen to start
	if(display.x < 225 && display.x > 15 && display.y < 251 && display.y > 69 && !started){
		start();
		started = true;
	}
	//Clear button
	if(display.x < 225 && display.x > 130 && display.y < 300 && display.y > 270 && started && !you_win_flag){
		disable_RIT();
		clear(false);
		reset_RIT();
		proximity_sensor();
		enable_RIT();
	}
	//Restart button
	if(display.x < 110 && display.x > 15 && display.y < 300 && display.y > 270 && started){
		disable_RIT();
		clear(true);
		draw_grid(1,false); //Cancella la griglia
		//Se precedentemente la partita era vinta, cancella la scritta you_win
		if(you_win_flag)
			you_win();
		GUI_Text(21,152, (uint8_t *) "Touch to start a new game", Black, White);
		reset_RIT(); // Il RIT viene abilitato quando viene richiamata la funzione start()
		started = false;
	}

	LPC_TIM0->IR = 1; //clear interrupt flag
	enable_timer(0);
  return;
}

/******************************************************************************
**                            End Of File
******************************************************************************/
