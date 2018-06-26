/*
 * Project 5.1.c
 *
 * Created: 3/16/2018 1:46:29 PM
 * Author : Ryan
 */ 

#include <avr/io.h>
#include "avr.h"

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

void
wait_avr2(unsigned short msec)
{
	TCCR0 = 3;
	while (msec--) {
		TCNT0 = (unsigned char)(256 - (XTAL_FRQ / 64) * 0.001);
		SET_BIT(TIFR, TOV0);
		WDR();
		while (!GET_BIT(TIFR, TOV0));
	}
	TCCR0 = 0;
}

struct note{
	int freq;
	int duration;
};

void play_note(int freq, int duration){
	int k = 1.7*durationTable[duration]/periodTable[freq];

	if (freq == REST){
		int msec = 1000*durationTable[duration]*1.6;
		wait_avr2(msec);
	}

	int i;
	for (i = 0; i < k; i++){
		SET_BIT(PORTA, 0);
		wait_avr(waitTable[freq]);
		CLR_BIT(PORTA, 0);
		wait_avr(waitTable[freq]);
	}
}

void play_music(const struct note song[], int songSize){
	int i;
	for (i = 0; i < songSize; i++){
		play_note(song[i].freq, song[i].duration);
	}
}



static const struct note Song1[size1] = {
	{C5, E},{C5, E},{E5, Q},{C5, E},{C5, E},{F5, Q},
	{C5, E},{C5, E},{G5, Q},{C5, E},{C5, E},{F5, Q},
	{C6, E},{B5F, E},{A5, E},{B5F, E},{A5, E},{F5, E},{C5, Q},
	{D5, E},{C5, E},{D5, E},{E5, E},{F5, E},{C6, E},{REST,E},
	{D6, E},{C6, E},{D6, E},{E6, E},{F6, E},{C6, E},{REST,E},
	{B5F, E},{A5, E},{G5, E},{F5, E},{D6, Q},{C6, Q},
	{C5, Q},{C5, Q},{C5, Q},{C5, Q},
	{G5, E},{F5, E},{E5, E},{D5, E},{E5, E},{C5, E},{G4, E},{D5, E},
	{C5, Q},{C5, Q},{C5, Q},{C5, Q},
	{G4, E},{D5, E},{E5, E},{D5, E},{G4, E},{C5, E},{F5, E},{F5, E},
	{C6, DQ},{F6, DQ},{E6, Q},
	{D6, E},{C6, E},{A5, E},{G5, E},{A5, E},{F5, Q},{D6, E},
	{C6, DQ},{F6, DQ},{E6, Q},
	{D6, E},{C6, E},{A5, E},{F5, Q},{C5, E},{G5, E},{F5, E},
};

static const struct note Song3[size3] = {
{C5, Q},{A5, S},{G5, E},{C5, SH},
{C5, Q},{A5, S},{G5, E},{D5, SQ},{D5, E},{C5, E},
{C5, Q},{A5, S},{G5, E},{C5, SDQ},{E5, E},
{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},

{C5, Q},{A5, S},{G5, E},{C5, SH},
{C5, Q},{A5, S},{G5, E},{D5, SQ},{E5, E},{G5, E},
{G5, Q},{A5, S},{G5, E},{C5, SDQ},{E5, E},
{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},{E5, E},{C5, S},{D5, S},{C5, Q}
};

int main(void)
{
    /* Replace with your application code */
	SET_BIT(DDRA, 0);
	wait_avr2(2500);
    while (1) 
    {
		if (GET_BIT(PINA, 3)){
			play_music(Song3,size3);
			wait_avr2(2000);
		}
		else if (GET_BIT(PINA, 4)){
			play_music(Song1, size1);
		}
    }
}

