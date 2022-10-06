#include "lpc17xx.h"
#include "RIT.h"
#include "../game.h"
#include "../timer.h"
#include <stdbool.h>
#define one_sec 20 

extern char direction;
volatile int mod;
extern int obs;

void RIT_IRQHandler (void){		
	static int switch_button=0;
	static int rotate_select=0;
	static int run_select=0;
	reset_RIT();
	
//***************************************************
//*******************Switch mode*********************
//***************************************************
		
	if((LPC_GPIO1->FIOPIN & (1<<25)) == 0 && switch_button == 0){	
		/* Joytick Select pressed */
		switch_button++;
		run_select = 0;
		if(mod==1)
			mod=2;
		else
			mod=1;
		draw_arrow();
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
		return;
	}
	if(!((LPC_GPIO1->FIOPIN & (1<<25)) == 0) && switch_button == 1){
		switch_button=0;
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
		return;
	}
//***************************************************
	
//***************************************************
//*******************EXPLORE*************************
//***************************************************
	
	if(mod == 1){
		
		//ROTATE IN MODALITY 1
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && direction != 'S'){	
			if(rotate_select == 0){
				rotate_select++;
				switch_direction('S');
			}
			goto exit;
		}
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && direction != 'W'){	
			if(rotate_select == 0){
				rotate_select++;
				switch_direction('W');
			}
			goto exit;
		}
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && direction != 'N'){	
			if(rotate_select == 0){
				rotate_select++;
				switch_direction('N');
			}
			goto exit;
		}
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && direction != 'E'){	
			if(rotate_select == 0){
				rotate_select++;
				switch_direction('E');
			}
			goto exit;
		}
		else{
			rotate_select=0;
			goto exit;
		}
	}
//***************************************************
	
//***************************************************
//***********************MOVE************************
//***************************************************
	if(mod == 2){
		int value;
		value = LPC_GPIO1->FIOPIN;
		if((value & 0xFFFF0000)>>16 == 0xFFFF)//Condizione verificata quando il joystick non è attivo su nessuna direzione
			run_select = 0;
		//Se viene selezionata una direzione delle seguenti (NE-NW-SE-SW) il conteggio viene azzerato
		if(	((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && ((LPC_GPIO1->FIOPIN & (1<<27)) == 0 || (LPC_GPIO1->FIOPIN & (1<<28)) == 0))
			|| ((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && ((LPC_GPIO1->FIOPIN & (1<<26)) == 0 || (LPC_GPIO1->FIOPIN & (1<<29)) == 0))
			|| ((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && ((LPC_GPIO1->FIOPIN & (1<<28)) == 0 || (LPC_GPIO1->FIOPIN & (1<<27)) == 0))
			|| ((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && ((LPC_GPIO1->FIOPIN & (1<<26)) == 0 || (LPC_GPIO1->FIOPIN & (1<<29)) == 0)))
			run_select=0;
		
		
		//JOYSTICK - DOWN
		if((LPC_GPIO1->FIOPIN & (1<<26)) == 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && run_select == one_sec){	
			run_select = 0;
			if(direction != 'S'){//Questa condizione evita di eseguire la funzione switch_direction se la direzione corrente è quella giusta
				switch_direction('S');
				run();
				goto exit;
			}
			if(obs!=0)//Questa condizione evita di eseguire la funzione run se ci si trova davanti ad un ostacolo
				run();
			goto exit;
		}
		//JOYSTICK - LEFT
		if((LPC_GPIO1->FIOPIN & (1<<27)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && run_select == one_sec){	
			run_select = 0;
			if(direction != 'W'){//Questa condizione evita di eseguire la funzione switch_direction se la direzione corrente è quella giusta
				switch_direction('W');
				run();
				goto exit;
			}
			if(obs!=0)//Questa condizione evita di eseguire la funzione run se ci si trova davanti ad un ostacolo
				run();
			goto exit;
		}
		//JOYSTICK - UP
		if((LPC_GPIO1->FIOPIN & (1<<29)) == 0 && (LPC_GPIO1->FIOPIN & (1<<28)) != 0 && (LPC_GPIO1->FIOPIN & (1<<27)) != 0 && run_select == one_sec){	
			run_select = 0;
			if(direction != 'N'){//Questa condizione evita di eseguire la funzione switch_direction se la direzione corrente è quella giusta
				switch_direction('N');
				run();
				goto exit;
			}
			if(obs!=0)//Questa condizione evita di eseguire la funzione run se ci si trova davanti ad un ostacolo
				run();
			goto exit;
		}
		//JOYSTICK - RIGHT
		if((LPC_GPIO1->FIOPIN & (1<<28)) == 0 && (LPC_GPIO1->FIOPIN & (1<<26)) != 0 && (LPC_GPIO1->FIOPIN & (1<<29)) != 0 && run_select == one_sec){	
			run_select = 0;
			if(direction != 'E'){//Questa condizione evita di eseguire la funzione switch_direction se la direzione corrente è quella giusta
				switch_direction('E');
				run();
				goto exit;
			}
			if(obs!=0)//Questa condizione evita di eseguire la funzione run se ci si trova davanti ad un ostacolo
				run();
			goto exit;
		}
		else{
			run_select++;
			goto exit;
		}
	}
	
	exit:
		LPC_RIT->RICTRL |= 0x1;	/* clear interrupt flag */
		return;
}
	

/******************************************************************************
**                            End Of File
******************************************************************************/

