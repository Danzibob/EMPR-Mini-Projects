#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_gpio.h"

// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};
int allLEDs= (1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4);

void delayms(int);
void displayNibble(char);
int oneHot2Bin(uint8_t);

int timems = 0;

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

int oneHot2Bin(uint8_t N){
	if(N == 0x08) return 3;
	if(N == 0x04) return 2;
	if(N == 0x02) return 1;
	if(N == 0x01) return 0;
	return -1;
}
