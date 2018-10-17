#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"
#include "i2c.c"
#include "LCD.c"

#define KEYPAD_I2C_ADDRESS   (0x21)

uint8_t readKeypad(void);
char getKey(void);

Status i2c_status;

char keypadKeys[4][4] = {
	{'D','#','0','*'},
	{'C','9','8','7'},
	{'B','6','5','4'},
	{'A','3','2','1'}
};

void main (void)
{
	// Initialize SysTick interrupts every ms
	SysTick_Config(SystemCoreClock / 1000);

	// Initialize stuff
	GPIO_SetDir(1, 0xFFFFFFFF, 1);
	serial_init();
	setupI2C();
	LCDSetup();
	LCDClear();

	// ---=== TASK 2 ===---

	// Set LCD to wrote on the top line
	LCDTopLine();
	// Array to hold message bytes
	uint8_t helloWorld[12];
	// String to be converted
	char helloString[] = "Hello World";
	// Populate byte array from string
	text2LCDBytes(helloString, 12, helloWorld);
	// Send the bytes to the LCD controller
	i2c_status = sendBytes(LCD_I2C_ADDRESS, helloWorld, 12);

	delayms(1000);

	// Clear the LCD screen
	LCDClear();

	// Write Hello to the top line
	LCDTopLine();
	uint8_t hello[6];
	char helloStr[] = "Hello";
	text2LCDBytes(helloStr, 6, hello);
	i2c_status = sendBytes(LCD_I2C_ADDRESS, hello, 6);

	// Write World to the bottom line
	LCDBottomLine();
	uint8_t world[6];
	char worldStr[] = "World";
	text2LCDBytes(worldStr, 6, world);
	i2c_status = sendBytes(LCD_I2C_ADDRESS, world, 6);

	delayms(2000);

	LCDClear();
	LCDTopLine();

	// ---=== TASK 3 ===---

	char lastKey = ' '; // Tracks the last char we entered - ' ' means no key pressed	
	int cursor = 0;     // Tracks where we are on the display, if we need to switch row 
	while(1)
	{
		// Get the character currently pressed
		char keypressed = getKey();
		// Check if this is different to the last time we checked
		if(lastKey != keypressed) 
		{
			lastKey = keypressed;
			if(keypressed != ' ') // If this is actually a key and not lack thereof
			{
				// If we've just (re)started typing, reset LCD
				if(cursor == 0){
					LCDClear();
					LCDTopLine();
				}

				// Move cursor and write character
				cursor++;
				LCDWriteChar(keypressed);

				// Check if we need to switch lines, and do so
				if(cursor == 16){
					LCDBottomLine();
				} else if (cursor == 32){
					cursor = 0;
				}
			}
		}
	}

}

Status writeKeypad(int col_num)
{
	uint8_t byte[1] = {0xff};
	uint8_t row = 0x01 << col_num;
	// Turn off only the nth bit
	byte[0] = byte[0] & (~row);
	Status s = sendBytes(KEYPAD_I2C_ADDRESS, byte, 1);
	return s;
}

uint8_t readKeypad(void)
{
	uint8_t data[1];
	readBytes(KEYPAD_I2C_ADDRESS, data, 1);
	// Convert read byte to one-hot column representation
	return (~data[0] & 0xf0) >> 4;
}

int oneHot2Bin(uint8_t N){
	if(N == 0x08) return 3;
	if(N == 0x04) return 2;
	if(N == 0x02) return 1;
	if(N == 0x01) return 0;
	return -1;
}

char getKey(void)
{
	int row;
	for(row = 0; row < 4; row++)
	{
		writeKeypad(row);
		delayms(1);
		uint8_t keypadBits = readKeypad();
		int val = oneHot2Bin(keypadBits);
		if(val != -1) // -1 indicates no keys pressed
		{
			// Breaks out of the loop to return - no key rollover*
			return keypadKeys[row][val];
		}
		delayms(1);
	}
	// ' ' indicates no key was found to be pressed
	return ' ';
}

/* There _is_ rollover, but only if you're moving down the keyboard */
