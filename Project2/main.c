#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"
#include "i2c.c"

// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};
int allLEDs= (1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4);

#define KEYPAD_I2C_ADDRESS   (0x21)
#define LCD_I2C_ADDRESS      (0x3B)

#define LCD_WAIT_MS          (10)

void delayms(int);
void displayNibble(char);
uint8_t * text2LCDBytes(char[], int, uint8_t[]);

void LCDClear(void);
void LCDTopLine(void);
void LCDBottomLine(void);
void LCDSetup(void);

int timems = 0;
Status i2c_status;

void main (void)
{
	// Initialize SysTick interrupts every ms
	SysTick_Config(SystemCoreClock / 1000);

	// Initialize stuff
	serial_init();
	setupI2C();
	LCDSetup();
	LCDClear();

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

	delayms(2000);

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

}

void delayms(int ms)
{
	int start = timems;
	while(timems - start < ms){}
}

void SysTick_Handler(void)
{
	timems++;
}

void displayNibble(char N)
{
	int digit;
	// Count down the digits, so MSB is first
	for(digit = 3; digit >= 0; digit--)
	{
		if(N & (1 << (3-digit)))
		{
			GPIO_SetValue(1, LEDs[digit]);
		} else
		{
			GPIO_ClearValue(1, LEDs[digit]);
		}
	}
	delayms(100);
}

uint8_t * text2LCDBytes(char string[], int len, uint8_t out[])
{
	out[0] = 0x40;
	int i;
	for(i = 0; i < len-1; i++)
	{
		// Check if the character is a letter (UPPER or lower)
		if((' ' <= string[i] && string[i] <= 'Z') || ('a' <= string[i] && string[i] <= 'z'))
		{
			out[i+1] = string[i] + 128;
		} 
		else
		{
			out[i+1] = string[i];
		}
	}
	return out;
}

void LCDTopLine(void)
{
	uint8_t topLine[] = {0x00, 0x80};
	sendBytes(LCD_I2C_ADDRESS, topLine, 2);
	delayms(LCD_WAIT_MS);
}

void LCDBottomLine(void)
{
	uint8_t topLine[] = {0x00, 0xC0};
	sendBytes(LCD_I2C_ADDRESS, topLine, 2);
	delayms(LCD_WAIT_MS);
}

void LCDClear(void)
{
	LCDTopLine();
	delayms(LCD_WAIT_MS);
	uint8_t clear[] = {0x40, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0};
	sendBytes(LCD_I2C_ADDRESS, clear, 17);
	delayms(LCD_WAIT_MS);
	LCDBottomLine();
	sendBytes(LCD_I2C_ADDRESS, clear, 17);
	delayms(LCD_WAIT_MS);
}

void LCDSetup(void)
{
	uint8_t setupBytes[] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02};
	sendBytes(LCD_I2C_ADDRESS, setupBytes, 11);
	delayms(200);
}