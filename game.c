//************************************************
//***** THIS FILE IMPLEMENTS THE GAME LOGIC ******
//************************************************

#include <stdbool.h>
#include "game.h"
#include ".\GLCD\GLCD.h"
#include ".\RIT\RIT.h"
#include ".\timer.h"



int maze[13][15] = {
	{2,0,0,0,0,0,0,0,0,0,0,0,0,0,2},
	{0,0,1,1,1,1,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0},
	{1,1,1,1,1,0,0,1,0,1,0,0,0,1,0},
	{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
  {1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
	{1,0,0,0,0,0,0,0,0,0,0,0,0,1,0},
  {1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{1,0,1,0,0,0,0,0,0,0,0,0,0,0,0},
	{2,0,1,0,0,0,0,0,0,0,0,0,0,0,2}	
};

//maze_explored tiene traccia delle caselle che vengono colorate, quindi degli ostacoli (rosso) e del percorso fatto dal robot (giallo). E' utile a velocizzare diversi processi.
int maze_explored[13][15];

int position[2];
volatile char direction;
int vertical_offset = 69, horizontal_offset = 15;
extern int mod;
volatile int obs = -1; //Inizializzata a -1 poiche' questo valore viene interpretato come se non ci fossero ostacoli sul percorso
volatile bool you_win_flag = false;

//La funzione di start inizializza posizione, direzione del robot e modalita'; cancella inoltre il messaggio “Touch screen to start a new game” e
//disegna la griglia di gioco. Richiama la funzione proximity_sensor() per scovare gli ostacoli posti davanti il robot e la funzione draw_arrow() per disegnare il robot.
void start(){
	position[0] = 7; 
	position[1] = 7; 
	direction = 'E';
	mod = 2;
	draw_grid(0,true); //Cancella il messaggio e disegna la griglia
	draw_arrow();
	proximity_sensor();
	enable_RIT();
}
//La funzione di run effettua un controllo preliminare sulla cella verso cui si intende spostare il robot per accertarsi che questa non sia un ostacolo o 
//il perimetro della griglia. In questa funzione viene stabilita la condizione di vittoria; nel caso in cui si è raggiunta una cella di uscita, 
//il RIT viene disabilitato in modo tale da inibire le funzionalità del joystick e viene richiamata la funzione you_win().
void run(){
	int next[2];
	bool flag = true;
	
	switch (direction){
		//In base alla direzione si controlla la casella successiva per vedere se e' possibile avanzare
		case ('N'):
			if(position[1]==0)
				flag = false;
			else{
				next[0] = position[0];
				next[1] = position[1]-1;
				break;
			}
		case ('S'):
			if(position[1]==12)
				flag = false;
			else{
				next[0] = position[0];
				next[1] = position[1]+1;
				break;
			}
		case ('E'):
			if(position[0]==14)
				flag = false;
			else{
				next[0] = position[0]+1;
				next[1] = position[1];
				break;
			}
		case ('W'):
			if(position[0]==0)
				flag = false;
			else{
				next[0] = position[0]-1;
				next[1] = position[1];
				break;
			}
	}
	if(flag){
		int result = maze[next[1]][next[0]];
		switch (result){
			case (0): // run
				print_cell(position[0],position[1],Yellow);	//Colora percorso
				maze_explored[position[1]][position[0]]=-1; //Aggiorna la versione del labirinto esplorato tenendo traccia della cella colorata di giallo (percorso)
				position[0] = next[0];
				position[1] = next[1];
				//In questo modo si evita che la freccia venga cancellata nel caso in cui venisse premuto Clear,
				//e la cella corrente fosse contrassegnata nella copia del labirinto esplorato, come esplorata (!=0)
				if(maze_explored[position[1]][position[0]]!=0){	
					maze_explored[position[1]][position[0]]=0;	
					print_cell(position[0],position[1],White);
				}
				draw_arrow();
				proximity_sensor();
				return;
			case (1): // Obstacle
				return; 							
			case (2): // Win
				disable_RIT();
				reset_RIT();
				clear(true);
				you_win();
				return;								
			default:
				return;
		}
	}
	else 
		return;
}

//La funzione di switch_direction modifica la posizione del robot in base alla direzione indicata nel valore “d”,
//richiama inoltre la funzione draw_arrow() per disegnare il robot, e la funzione proximity_sensor().
void switch_direction(char d){
	  direction = d;
		obs = -1;
		print_cell(position[0],position[1],White);	//clear cell
		draw_arrow();
		proximity_sensor();
	
}


//La funzione proximity_sensor individua gli ostacoli posti ad una distanza massima di 5 celle di distanza dal robot. 
//Quando un ostacolo viene individuato, questo viene memorizzato in una copia inizialmente inizializzata a zero del labirinto, 
//in modo tale da tener traccia di tutti gli ostacoli trovati e facilitare il lavoro della funzione clear() che pulisce la griglia 
//alla pressione del pulsante Clear. In questa funzione modifica la variabile obs, posta a zero quando il robot si trova davanti ad 
//un ostacolo, in modo tale da evitare di eseguire la funzione run() quando ci si trova davanti ad un ostacolo.
void proximity_sensor(){
	int i;
	switch (direction){
		
		//DIRECTION - NORD
		case ('N'):
			if(position[1] == 0){	//Up perimeter
				obs = 0;	
				goto exit;
			}
			for(i=1; i<7; i++){
				if(maze[position[1]-i][position[0]]==2){ //Casella di vittoria
					obs = -1;	//Quando verrà sottratto ad Obs 1, il valore di distanza sarà -1 qundi No blink
					goto exit;
				}
				//Condizione verificata se la prossima casella e' l'ultima prima del perimetro o se e' un ostacolo
				if((position[1]-i == 0)|| maze[position[1]-i][position[0]]==1){
					//Questa condizione e' verificata se la prossima cella e' un ostacolo
					if(maze[position[1]-i][position[0]]==1){ 
						if(maze_explored[position[1]-i][position[0]]==0){
							print_cell(position[0],position[1]-i,Red); //Colora ostacolo
							maze_explored[position[1]-i][position[0]]=1; //Aggiorna la versione del labirinto esplorato tenendo traccia della cella colorata di rosso (ostacolo)
						}
						obs=i-1;
						goto exit;
					}
					if(position[1]-i == 0){
						obs = i;
						goto exit;
					}
				}
			}
			goto exit;
			
		//DIRECTION - EST
		case ('E'):
			if(position[0] == 14){ 	//Right perimeter
				obs = 0;	
				goto exit;
			}
			for(i=1; i<7; i++){
				if(maze[position[1]][position[0]+i]==2){//Casella di vittoria
					obs = -i;	//Quando verrà sottratto ad Obs 1, il valore di distanza sarà -1 qundi No blink
					goto exit;
				}
				//Condizione verificata se la prossima casella e' l'ultima prima del perimetro o se e' un ostacolo
				if((position[0]+i == 14)|| maze[position[1]][position[0]+i]==1){
					//Questa condizione e' verificata se la prossima cella e' un ostacolo
					if(maze[position[1]][position[0]+i]==1){  
						if(maze_explored[position[1]][position[0]+i]==0){ //L'ostacolo viene colorato soltanto se non e' gia' colorato
							print_cell(position[0]+i,position[1],Red); //Colora ostacolo
							maze_explored[position[1]][position[0]+i]=1; //Aggiorna la copia del labirinto esplorato
						}
						obs=i-1;
						goto exit;
					}
					if(position[0]+i == 14){
						obs=i;
						goto exit;
					}
				}
			}
			goto exit;
		
		//DIRECTION - SOUTH	
		case ('S'):
			if(position[1] == 12){		//Down perimeter
				obs = 0;	
				goto exit;
			}
			for(i=1; i<7; i++){
				if(maze[position[1]+i][position[0]]==2){//Casella di vittoria
					obs = -i;	//Quando verrà sottratto ad Obs 1, il valore di distanza sarà -1 qundi No blink
					goto exit;
				}
				//Condizione verificata se la prossima casella e' l'ultima prima del perimetro o se e' un ostacolo
				if((position[1]+i == 12)|| maze[position[1]+i][position[0]]==1){
					//Questa condizione e' verificata se la prossima cella e' un ostacolo
					if(maze[position[1]+i][position[0]]==1){
						if(maze_explored[position[1]+i][position[0]]==0){ //L'ostacolo viene colorato soltanto se non e' gia' colorato
							print_cell(position[0],position[1]+i,Red); //Colora ostacolo
							maze_explored[position[1]+i][position[0]]=1; //Aggiorna la versione del labirinto esplorato tenendo traccia della cella colorata di rosso (ostacolo)
						}
						obs=i-1;
						goto exit;
					}
					if(position[1]+i == 12){
						obs=i;
						goto exit;
					}
				}
			}
			goto exit;
			
		//DIRECTION - WEST	
		case ('W'):	
			if(position[0] == 0){	//Left perimeter
				obs = 0;
				goto exit;
			}
			for(i=1; i<7; i++){
				if(maze[position[1]][position[0]-i]==2){//Casella di vittoria	/////// GESTIRE VITTORIA
					obs = -i;	
					goto exit;
				}
				//Condizione verificata se la prossima casella e' l'ultima prima del perimetro o se e' un ostacolo
				if((position[0]-i==0) || maze[position[1]][position[0]-i]==1){
					//Questa condizione e' verificata se la prossima cella e' un ostacolo
					if(maze[position[1]][position[0]-i]==1){  
						if(maze_explored[position[1]][position[0]-i]==0){ //L'ostacolo viene colorato soltanto se non e' gia' colorato
							print_cell(position[0]-i,position[1],Red); //Colora ostacolo
							maze_explored[position[1]][position[0]-i]=1; //Aggiorna la copia del labirinto esplorato
						}
						obs=i-1;
						goto exit;
					}
					if((position[0]-i==0)){
						obs=i;
						goto exit;
					}
					
				}	
			}
	}
	exit: 
		return;
}

//La funzione draw_GUI disegna l’interfaccia di gioco, i pulsanti e la scritta “Blind Labirinth”.
void draw_GUI(){
	int x,y;
	//Background
	for(y=0; y<69; y++)
		LCD_DrawLine(0,y,240,y,Blue);
	for(y=252; y<270; y++)
		LCD_DrawLine(15,y,225,y,Blue);
	for(y=300; y<321; y++)
		LCD_DrawLine(15,y,225,y,Blue);
	for(x=0; x<15; x++)
		LCD_DrawLine(x,69,x,320,Blue);
	for(x=111; x<130; x++)
		LCD_DrawLine(x,270,x,300,Blue);
	for(x=226; x<241; x++)
		LCD_DrawLine(x,69,x,320,Blue);
	
	GUI_Text(59,40, (uint8_t *) "BLIND LABIRINTH", White, Blue);
		
	//Button1
	LCD_DrawLine(15,270,15,300,Black);
	LCD_DrawLine(15,270,110,270,Black);
	LCD_DrawLine(15,300,110,300,Black);
	LCD_DrawLine(110,270,110,300,Black);
	GUI_Text(35,277, (uint8_t *) "Restart", Blue, White);
	//Button2
	LCD_DrawLine(130,270,130,300,Black);
	LCD_DrawLine(130,270,225,270,Black);
	LCD_DrawLine(130,300,225,300,Black);
	LCD_DrawLine(225,270,225,300,Black);
	for(y=271; y<300; y++)
		LCD_DrawLine(131,y,224,y,Yellow);
	GUI_Text(159,277, (uint8_t *) "Clear", Black, Yellow);
	
	GUI_Text(21,152, (uint8_t *) "Touch to start a new game", Black, White);
}

//La funzione draw_grid() in base al valore di z, disegna (z=0) o cancella (else) la griglia, inoltre se la variabile message è true, cancella il messaggio di start.
void draw_grid(int z, bool message){
	int x,y;
	uint16_t color;
	//Disegna griglia
	if(z==0)
		color = Black;
	//Cancella griglia
	else
		color = White;
	//Clear the message
	if(message)
		for(y=152; y<180; y++)
			LCD_DrawLine(21,y,224,y,White);
	//Vertical lines
	for(x=horizontal_offset; x<=225; x=x+14)
		LCD_DrawLine(x,vertical_offset,x,251,color);
	//Orizontal lines
	for(y=vertical_offset; y<=251; y=y+14)
		LCD_DrawLine(horizontal_offset,y,225,y,color);
}
//Poiché la libreria fornita per disegnare le linee, presenta in determinati casi dei bug, è stata scritta la funzione draw_arrow() che
//permette di disegnare un triangolo (robot) in base a posizione e direzione correnti nel gioco.
void draw_arrow(){
	int x0,x1,x2,y0,y1,y2;
		int y0_temp;
		int y1_temp;
		int x0_temp;
		int x1_temp;
		uint16_t color;
	//Colora in modo diverso in base alla modalità
	if(mod == 1)
		color = Green;
	else
		color = Red;
		switch(direction){
			case 'E':
					x0=((position[0]*14)+4)+horizontal_offset;
					y0=((position[1]*14)+2)+vertical_offset;
					x1= x0;
					y1=((((position[1]+1)*14)-2))+vertical_offset;
					x2=(((position[0]+1)*14))+horizontal_offset;
					y2=((y1-y0)/2)+y0;
					break;
			case 'W':
				x0=(((position[0]+1)*14)-4)+horizontal_offset;
				y0=(((position[1])*14)+2)+vertical_offset;
				x1= x0;
				y1=((((position[1]+1)*14)-2))+vertical_offset;
				x2=(((position[0])*14))+horizontal_offset;
				y2=((y1-y0)/2)+y0;
				break;
			case 'N':
				x0=(((position[0]+1)*14)-2)+horizontal_offset;
				y0=(((position[1]+1)*14)-4)+vertical_offset;
				x1=(((position[0])*14)+2)+horizontal_offset;
				y1=y0;
				x2=((x0-x1)/2)+x1;
				y2=(((position[1])*14))+vertical_offset;
				break;
			case 'S':
				x0=(((position[0]+1)*14)-2)+horizontal_offset;
				y0=(((position[1])*14)+4)+vertical_offset;
				x1=(((position[0])*14)+2)+horizontal_offset;
				y1=y0;
				x2=((x0-x1)/2)+x1;
				y2=(((position[1]+1)*14))+vertical_offset;
				break;
		}
		y0_temp = y0;
		y1_temp = y1;
		x0_temp = x0;
		x1_temp = x1;
		switch(direction){
			case 'E':
				while(x0_temp <= x2){
					while(y0_temp<=y1_temp){
						LCD_SetPoint(x0_temp,y0_temp,color);
						y0_temp++;
					}
					y0++;
					y1--;
					y0_temp = y0;
					y1_temp = y1;
					x0_temp++;
				}
				return;
			case 'W':
				while(x0_temp >= x2){
					while(y0_temp<=y1_temp){
						LCD_SetPoint(x0_temp,y0_temp,color);
						y0_temp++;
					}
					y0++;
					y1--;
					y0_temp = y0;
					y1_temp = y1;
					x0_temp--;
				}
				return;
			case 'N':
				while(y0_temp >= y2){
					while(x0_temp>=x1_temp){
						LCD_SetPoint(x1_temp,y0_temp,color);
						x1_temp++;
					}
					x1++;
					x0--;
					x0_temp = x0;
					x1_temp = x1;
					y0_temp--;
				}
				return;
			case 'S':
				while(y0_temp <= y2){
					while(x0_temp>=x1_temp){
						LCD_SetPoint(x1_temp,y0_temp,color);
						x1_temp++;
					}
					x1++;
					x0--;
					x0_temp = x0;
					x1_temp = x1;
					y0_temp++;
				}
				return;
			}
	}

	//La funzione print_cell, colora una cella passandole le coordinate e il colore.
void print_cell(int x0, int y0, uint16_t color){
	int x1,y,y1;
	x1=((x0*14)+1)+horizontal_offset;
	y1=((((y0)*14)+1))+vertical_offset;
	for(y=y1; y<y1+13; y++)
		LCD_DrawLine(x1,y,x1+12,y,color);
}
//La funzione clear pulisce la griglia e se la variabile flag è vera pulisce la figura del robot.
//Utilizzando una versione del labirinto che tiene traccia delle caselle colorate (maze_explored), questa funzione e' molto piu'veloce.
void clear(bool flag){
	int i,j;
	for(i=0;i<15;i++){
		for(j=0;j<13;j++){
			if(maze_explored[j][i]!=0){
				maze_explored[j][i]=0;
				print_cell(i,j,White);
			}
		}
	}
	if(flag)
		print_cell(position[0],position[1],White);
}

//La funzione you_win in base alla variabile globale you_win_flag, disegna (true) o cancella (false) la scritta “YOU WIN”.
void you_win(){
	uint16_t color;
	//Disegna la scritta
	if(!you_win_flag){
		color = Green;
		you_win_flag = true;
	}
	//Cancella la scritta
	else{
		color = White;
		you_win_flag = false;
	}
	//Y
	print_cell(2,3,color);
	print_cell(2,4,color);
	print_cell(3,4,color);
	print_cell(3,5,color);
	print_cell(4,3,color);
	print_cell(4,4,color);
	//O
	print_cell(6,3,color);
	print_cell(6,4,color);
	print_cell(6,5,color);
	print_cell(7,3,color);
	print_cell(7,5,color);
	print_cell(8,3,color);
	print_cell(8,4,color);
	print_cell(8,5,color);
	//U
	print_cell(10,3,color);
	print_cell(10,4,color);
	print_cell(10,5,color);
	print_cell(11,5,color);
	print_cell(12,3,color);
	print_cell(12,4,color);
	print_cell(12,5,color);
	//W
	print_cell(1,7,color);
	print_cell(1,8,color);
	print_cell(1,9,color);
	print_cell(2,8,color);
	print_cell(3,7,color);
	print_cell(4,8,color);
	print_cell(5,7,color);
	print_cell(5,8,color);
	print_cell(5,9,color);
	//I
	print_cell(7,7,color);
	print_cell(7,8,color);
	print_cell(7,9,color);
	//N
	print_cell(9,7,color);
	print_cell(9,8,color);
	print_cell(9,9,color);
	print_cell(10,7,color);
	print_cell(11,8,color);
	print_cell(12,9,color);
	print_cell(13,7,color);
	print_cell(13,8,color);
	print_cell(13,9,color);
}

/******************************************************************************
**                            End Of File
******************************************************************************/



