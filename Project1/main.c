#include "LPC17xx.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"

// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};
int allLEDs= (1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4);

// Allocate variables
int timems = 0;
char binString[4];

// Function Prototypes
void delayms(int);
void displayNibble(char);
void task(void);

// Actual Functions

int main(void)
{
	// Initialize SysTick interrupts every ms
	SysTick_Config(SystemCoreClock / 1000);

	// Initialize LED GPIO to out
	GPIO_SetDir(1, 0xFFFFFFFF, 1);

	// Initalize serial
	serial_init();

	task();

	return 0;
}

void task(void)
{
	write_usb_serial_blocking("Starting Count\n\r",16);

        // Count to 15
        int i;
        for(i = 0; i < 16; i++)
        {
                char outputStr[13];
                displayNibble(i); // Updates global binString
                sprintf(outputStr, "%2d %2X %04s\n\r", i, i, binString);
                write_usb_serial_blocking(outputStr,13);
                delayms(1000);
        }

        write_usb_serial_blocking("Finished Count\n\r",16);

	// Reset to zero
        displayNibble(0);
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
			binString[digit] = '1';
		} else
		{
			GPIO_ClearValue(1, LEDs[digit]);
			binString[digit] = '0';
		}
	}
}
