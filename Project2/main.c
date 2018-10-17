#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"
#include "i2c.c"
#include "LCD.c"

// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};
int allLEDs= (1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4);

#define KEYPAD_I2C_ADDRESS   (0x21)


void delayms(int);
void displayNibble(char);

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
