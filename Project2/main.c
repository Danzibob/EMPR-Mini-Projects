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

void delayms(int);
void displayNibble(char);

int timems = 0;

void main (void)
{
	// Initialize SysTick interrupts every ms
	SysTick_Config(SystemCoreClock / 1000);
	// Initalize serial
	serial_init();
	// Initialize I2C
	setupI2C();

	Status i2c_status;

	uint8_t setupBytes[] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02};
	i2c_status = sendBytes(LCD_I2C_ADDRESS, setupBytes, 11);
	delayms(1000);

	uint8_t data_out[] = {0x00, 0x00};

	//Clear the display
	data_out[0] = 0x00;
	data_out[1] = 0x01;
	i2c_status = sendBytes(LCD_I2C_ADDRESS, data_out, 2);
	delayms(1000);

	data_out[0] = 0x00;
	data_out[1] = 0x80;
	i2c_status = sendBytes(LCD_I2C_ADDRESS, data_out, 2);
	delayms(1000);
	// Write spaces to screen
	uint8_t clear[] = {0x40, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0};
	i2c_status = sendBytes(LCD_I2C_ADDRESS, clear, 17);
	delayms(1000);

	// Write Instruction reg
	data_out[0] = 0x00;
	data_out[1] = 0x80;
	i2c_status = sendBytes(LCD_I2C_ADDRESS, data_out, 2);
	delayms(1000);

	// Write message
	uint8_t helloWorld[] = {0x40, 0xC8, 0xE5, 0xEC, 0xEC, 0xEF};
	i2c_status = sendBytes(LCD_I2C_ADDRESS, helloWorld, 6);
	delayms(1000);
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