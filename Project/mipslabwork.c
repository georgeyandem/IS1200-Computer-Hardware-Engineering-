/* mipslabwork.c

   This file written 2015 by F Lundevall
   Updated 2017-04-21 by F Lundevall

   This file should be changed by YOU! So you must
   add comment(s) here with your name(s) and date(s):

   This file modified 2017-04-31 by Ture Teknolog

   For copyright and licensing, see file COPYING */

#include <stdint.h>	 /* Declarations of uint_32 and the like */
#include <pic32mx.h> /* Declarations of system-specific addresses etc */
#include "mipslab.h" /* Declatations for these labs */


/* Interrupt Service Routine */

void user_isr(void)
{
  IFSCLR(0) = 0x100; 
  interrupt = 1;
}

volatile int* pe = (volatile int*) 0xbf886110;   // LED:s, used to control the state of LED:s. Satte den utanför eftersom den måste va global då den används för score också.

/* Lab-specific initialization goes here */

void labinit(void)
{
   volatile int *te = (volatile int*) 0xbf886100;     
   *te &= ~0x00ff;                                    
   TRISD = 0x0fe0;                                   
    
   *pe = 0x0;

   T2CONSET = 0x70;                                   // 111 boken- sett precal till 256 genom precal kan vi devidera clock frekvensen och innan vi ge det till timer
   PR2 = (80000000/256) / 10;                         // Kolla på föreläsning 5 och kontrollera att den rymmer i 16-bits = 31250
   T2CONSET = 0X8000;                                 //STANDRA SIFFRA // Start timer
   T2CON = 0x0;                                       // Stop timer and clear control register
   TMR2 = 0x0;                                        // Clear timer register

   /* Denna del används för att enable interrupt på timer2
      för att veta vilka bitar som är relevanta har "family sheet" använts */

   IEC(0) = 0x100;          // set bit 8
   IPC(2) = IPC(2) | 0x10;  // prioritet
   enable_interrupt();     
  
  return;
}





/* GAME STUFF FROM HERE */



/* 	saker som används för att bestämma flowet genom programmet 	*/


// while loop state-controll variables
int startUpScreen = 1;           
int gameCurrentlyRunning = 0;

// Startposition för square, samt x-positionerna för pipes (obstacles)
int squareXPos = 8;             
int squareYPos = 3;            

int pipeXPositions[] = {40, 70, 100};			
int pipeUpDownLen[][2] = { {4, 5}, {6, 5}, {7, 4} };		
int numPipes = sizeof(pipeXPositions) / sizeof(pipeXPositions[0]);

int squareCrashControll[3];					

int score;			


/* Text som visas första gången spelet startar upp och när man förlorar */

char* gameStartupText[] = { "--FLAPPY BOX--", "", "Button 4 - Start", "Button 2 - Fly"};

char* gameoverText[] = {"--YOU CRASHED!--", "", "Score - see LEDs", "Btn4 - restart!"};

/*	Funktion som återställer allt och förberedder för ett nytt spel */

void gameoverHandler()
{
	pipeXPositions[2] = 100;
	pipeXPositions[1] = 70;
	pipeXPositions[0] = 40;
	*pe = 0x00;
	
	squareYPos = 3;
	squareXPos = 8;
}


// GAME:

/* This function is called repetitively from the main program */

void labwork(void)
{

	// denna loop blir klar när man trycker på knapp 4, då börjar spelaet
	while (startUpScreen == 1 && (getbtns() & 0x04) != 0x04)
	{
		displayText(gameStartupText, sizeof(gameStartupText) / sizeof(gameStartupText[0]));
        display_update();
	}

	startUpScreen = 0;
	gameCurrentlyRunning = 1;
	int gameover;

	// denna loop börjar när man har krockat och "dör"
	while (gameover)
	{
		displayText(gameoverText, sizeof(gameoverText) / sizeof(gameoverText[0]));

		if ((getbtns() >> 2) == 1)
		{
			gameover = 0;
			gameoverHandler();	//återställer spelet
		}
	}
	
	clearDispalyString();
	char readyPlayer[3][9] = {"Ready?", "Set", "FLY!!!"};
	int i;
	for (i = 0; i < 3; i++) 
	{
		display_string(i, readyPlayer[i]);
		display_update();
		delay(700);
	}

	while (gameCurrentlyRunning == 1)
	{
		clearDispalyString();
		floorAndRoof();		
		drawSquare(squareXPos, squareYPos++);
		
	
		for (i = 0; i < numPipes; i++) 
		{
    		generateObstacles(pipeXPositions[i]--, pipeUpDownLen[i][0], pipeUpDownLen[i][1]);
    		if (pipeXPositions[i] == 0) 
				pipeXPositions[i] = 127;
		}

		display_image(0, icon);
		clearDispalyArray();
		delay(50);
		if (getbtns() == 1)
			squareYPos -= 2; 
		display_update();

		for (i = 0; i < numPipes; i++) 
		{
    		squareCrashControll[i] = squareCrash(pipeXPositions[i], pipeUpDownLen[i][0], pipeUpDownLen[i][1], squareXPos, squareYPos);
    		if (squareCrashControll[i] == 0) 
			{
        		gameCurrentlyRunning = 0;
				gameover = 1;
        		break;
    		}
		}
		
		for (i = 0; i < numPipes; i++) 
		{
			score = scoreLED(pipeXPositions[i], squareXPos);
			if (score == 1) 
				*pe = *pe + 1;
		}
	}
	
}