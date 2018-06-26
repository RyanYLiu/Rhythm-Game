/*
 * Project 5.c
 *
 * Created: 3/5/2018 1:40:28 AM
 * Author : Ryan
 */ 

#include <avr/io.h>
#include <math.h>
#include <assert.h>
#include <string.h>
#include <stdio.h>
#include "avr.h"
#include "lcd.h"

#define size1 83
#define size3 55

#define W 0
#define H 1
#define DQ 2
#define Q 3
#define E 4
#define S 5
#define T 6
#define SH 7
#define SQ 8
#define SDQ 9

float durationTable[10] = {1.0, 0.5, 0.375, 0.25, 0.125, 0.0625, 0.25/3.0, 0.5625, 0.3125, 0.4375};

#define F4 0
#define G4 1
#define A4 2
#define B4F 3
#define B4  4
#define C5 5
#define D5F 6
#define D5 7
#define E5F 8
#define E5 9
#define F5 10
#define G5F 11
#define G5 12
#define A5F 13
#define A5 14
#define B5F 15
#define B5 16
#define C6 17
#define D6 18
#define E6F 19
#define E6 20
#define F6 21
#define REST 22

float periodTable[23] = {0.00286346, 0.00255105, 0.00227273, 0.00214517, 0.00202477, 0.00191113, 0.00180386, 0.00170262,
						 0.00160706, 0.00151686, 0.00143173, 0.00135137, 0.00127553, 0.00120394, 0.00113636, 0.00107258,
						 0.00101238, 0.00095556, 0.00085131, 0.00080353, 0.00075843, 0.00071586, 0};

int waitTable[23] = {1432, 1276, 1136, 1073, 1012, 956, 902, 851, 804, 758, 716, 676, 638, 602, 568, 536, 506, 478, 426, 402, 379, 358, 0};

void wait_avr2(unsigned short msec){
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

int is_pressed(int r, int c){
	DDRA = 0x00;
	PORTA = 0xF0;
	SET_BIT(DDRA, c);
	SET_BIT(PORTA, r+4);
	if(GET_BIT(PINA, r+4) == 0){
		return 1;
	}
	return 0;
}

int get_key(void){
	int key = 0;
	int r;
	int c;
	for (r = 0; r < 4; ++r){
		for (c = 0; c < 4; ++c){
			if(is_pressed(r,c)){
				key = r+(c*4)+1;
			}
		}
	}
	return key;
}


struct chart{
	char ledNum;
	int freq;
	int duration;
};

unsigned char hit_note(int button, int freq, int duration){
	unsigned char noteHit = 0;
	int k = 2.7*1000*durationTable[duration];

	if(freq == REST){
		int msec = 2.7*durationTable[duration]*1000;
		wait_avr2(msec);
	}

	int i;
	for (i = 0; i < k; i++){
		if (get_key() == button)
			noteHit = 1;
		wait_avr2(1);
	}
	return noteHit;
}

int play_game(const struct chart songChart[], int songSize, int songNum){
	int score = 0;
	int combo = 0;
	int multiplier = 1;
	char ui[16] = "Score:    0     ";
	pos_lcd(0,0);
	puts_lcd2(ui);
	pos_lcd(1,0);
	sprintf(ui, "Combo:    0     ");
	puts_lcd2(ui);
	int i;
	int button;
	SET_BIT(PORTC, songNum);
	for (i = 0; i < songSize; i++){
		if (songChart[i].freq == REST){
			int msec = durationTable[songChart[i].duration]*1000;
			wait_avr2(msec);
			continue;
		}
		else{
			SET_BIT(PORTC, songChart[i].ledNum);
			switch (songChart[i].ledNum){
				case 0:
					button = 1;
					break;
				case 1:
					button = 3;
					break;
				case 2:
					button = 6;
					break;
				case 3:
					button = 9;
					break;
				case 4:
					button = 11;
					break;
			}
			if(hit_note(button, songChart[i].freq, songChart[i].duration)){
				score += 10*multiplier;
				combo++;
			}
			else{
				combo = 0;
				multiplier = 1;
			}

			switch(combo){
				case 15:
					multiplier = 2;
					break;
				case 25:
					multiplier = 3;
					break;
				case 40:
					multiplier = 4;
					break;
			}
			CLR_BIT(PORTC, songChart[i].ledNum);
			pos_lcd(0,0);
			sprintf(ui, "Score: %4i     ", score);
			puts_lcd2(ui);
			pos_lcd(1,0);
			sprintf(ui, "Combo: %4i     ", combo);
			puts_lcd2(ui);
		}
	}
	CLR_BIT(PORTC, songNum);
	return score;
}

static const struct chart Song1Chart[size1] = {
	{0, C5, E},{1, C5, E},{2, E5, Q},{0, C5, E},{1, C5, E},{3, F5, Q},
	{0, C5, E},{1, C5, E},{4, G5, Q},{0, C5, E},{1, C5, E},{3, F5, Q},
	{4, C6, E},{3, B5F, E},{2, A5, E},{3, B5F, E},{2, A5, E},{1, F5, E},{0, C5, Q},
	{1, D5, E},{0, C5, E},{1, D5, E},{2, E5, E},{3, F5, E},{4, C6, E},{9, REST, Q},
	{2, D6, E},{1, C6, E},{2, D6, E},{3, E6, E},{4, F6, E},{1, C6, E},{9, REST, Q},
	{3, B5F, E},{2, A5, E},{1, G5, E},{0, F5, E},{3,D6, Q},{2,C6, Q},
	{0, C5, Q},{1, C5, Q},{2, C5, Q},{3, C5, Q},
	{4, G5, E},{3, F5, E},{2, E5, E},{1, D5, E},{2, E5, E},{1, C5, E},{0, G4, E},{2, D5, E},
	{0, C5, Q},{1, C5, Q},{2, C5, Q},{3, C5, Q},
	{0, G4, E},{1, D5, E},{2, E5, E},{1, D5, E},{0, G4, E},{2, C5, E},{3, F5, E},{4, F5, E},
	{1, C6, DQ},{3, F6, DQ},{2, E6, Q},
	{4, D6, E},{3, C6, E},{2, A5, E},{1, G5, E},{2, A5, E},{0, F5, Q},{4, D6, E},
	{1, C6, DQ},{3, F6, DQ},{2, E6, Q},
	{4, D6, E},{3, C6, E},{2, A5, E},{1, F5, Q},{0, C5, E},{2, G5, E},{1, F5, E},
};

static const struct chart Song3Chart[size3] = {
	{1, C5, Q},{3, A5, S},{2, G5, E},{1, C5, SH},
	{0, C5, Q},{2, A5, S},{1, G5, E},{0, D5, SQ},{1, D5, E},{0, C5, E},
	{1, C5, Q},{3, A5, S},{2, G5, E},{1, C5, SDQ},{3, E5, E},
	{3, E5, E},{1, C5, S},{2, D5, S},{3, E5, E},{1, C5, S},{2, D5, S},{3, E5, E},{0, C5, S},{1, D5, S},{2, E5, E},{1, C5, S},{2, D5, S},

	{1, C5, Q},{3, A5, S},{2, G5, E},{2, C5, SH},
	{0, C5, Q},{2, A5, S},{1, G5, E},{0, D5, SQ},{1, E5, E},{0, G5, E},
	{1, G5, Q},{3, A5, S},{2, G5, E},{1, C5, SDQ},{3, E5, E},
	{3, E5, E},{1, C5, S},{2, D5, S},{3, E5, E},{1, C5, S},{2, D5, S},{3, E5, E},{0, C5, S},{1, D5, S},{2, E5, E},{1, C5, S},{2, D5, S},{1, C5, Q}
};



int main(void)
{
    /* Replace with your application code */
	// LED, buttons, LCD
	MCUCSR = (1<<JTD);
	MCUCSR = (1<<JTD);
	ini_lcd();
	DDRC = 0xFF;

	// game start up
	int hiScore1 = 0;
	int hiScore2 = 0;
	char startMessage[16] = "   Welcome to   ";
	puts_lcd2(startMessage);
	sprintf(startMessage, "   Beatmania!   ");
	pos_lcd(1,0);
	puts_lcd2(startMessage);
	wait_avr2(2000);
	clr_lcd();

	// initial song selection interface
	sprintf(startMessage, "Choose a song   ");
	pos_lcd(0,0);
	puts_lcd2(startMessage);
	char songName[16] = "1. Oldale Town  ";
	unsigned char songSelect = 1;
	pos_lcd(1,0);
	puts_lcd2(songName);
	unsigned char gameStart = 0;
	while (1)
	{
		pos_lcd(0,0);
		sprintf(startMessage, "Choose a song   ");
		puts_lcd2(startMessage);
		switch(get_key()){
			case 1:
				songSelect = 1;
				break;
			case 2:
				songSelect = 2;
						break;
			case 4:
				gameStart = 1;
				break;
			case 16:
				clr_lcd();
				switch(songSelect){
					case 1:
						sprintf(startMessage, "HiScore: %4i   ", hiScore1);
						break;
					case 2:
						sprintf(startMessage, "HiScore: %4i   ", hiScore2);
						break;
				}
				pos_lcd(0,0);
				puts_lcd2(startMessage);
				wait_avr2(3000);
		}
		if (songSelect == 1)
			sprintf(songName, "1. Oldale Town  ");
		else if (songSelect == 2)
			sprintf(songName, "2. Shelter      ");
		pos_lcd(1,0);
		puts_lcd2(songName);

		if (gameStart){
			int temp = 0;
			clr_lcd();
			pos_lcd(0,7);
			put_lcd('3');
			wait_avr2(1000);
			pos_lcd(0,7);
			put_lcd('2');
			wait_avr2(1000);
			pos_lcd(0,7);
			put_lcd('1');
			wait_avr2(1000);
			clr_lcd();
			pos_lcd(0,0);
			sprintf(startMessage, "Score:");
			puts_lcd2(startMessage);
			pos_lcd(1,0);
			sprintf(startMessage, "Combo:");
			puts_lcd2(startMessage);
			wait_avr2(500);
			switch(songSelect){
				case 1:
					temp = play_game(Song1Chart, size1, 5);
					if (temp > hiScore1)
						hiScore1 = temp;
					break;
				case 2:
					temp = play_game(Song3Chart, size3, 6);
					if (temp > hiScore2)
						hiScore2 = temp;
					break;
			}
			gameStart = 0;
			wait_avr2(2000);
		}
	}
	wait_avr2(150);

}

