#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_pwm.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "serial.c"
#include "i2c.c"
#include "LCD.c"
#include "Keypad.c"
#include "adc.c"
#include "dac.c"
#include "pwm.c"

Status i2c_status;
int dutyCycle;

int INTTriggered = 0;

void EINT3_IRQHandler(void)
{
	// Clear the interrupt on the pin
	LPC_GPIOINT->IO0IntClr = (1 << 5);
	LPC_SC->EXTINT = (1 << 3);
	// Debounce 1000ms
	if(timems < 1000){
		write_usb_serial_blocking("Debouncing...\n\r",15);
		return;
	}
	timems = 0;
	write_usb_serial_blocking("TRIGGERED\n\r",11);
	INTTriggered = 1;
}

void main (void)
{
	SystemInit();
	// Initialize SysTick interrupts every ms
	SysTick_Config(SystemCoreClock / 1000);

	// Initialize stuff
	GPIO_SetDir(2, 0xFFFFFFFF, 1);
	serial_init();
	setupI2C();
	LCDSetup();
	LCDClear();
	ADCSetup();
	DACSetup();

	resetInterrupt();

	write_usb_serial_blocking("\n\r--START--\n\r",13);

	// Set P0.5 as GPIO input
	//LPC_PINCON->PINSEL0 &= ~(0x11 << 10);
	LPC_GPIO0->FIODIR &= ~(1 << 5);

	// Clear interrupts
	LPC_GPIOINT->IO0IntClr = (1 << 5);
	LPC_SC->EXTINT = (1 << 3);

	// Enable falling edge interrupt on P0.5
	LPC_GPIOINT->IO0IntEnF |= (1 << 5);

	// Enable GPIO interrupts
	NVIC_EnableIRQ(EINT3_IRQn);
	// Enable interrupts globally
	__enable_irq();

	while(1)
	{
		int i;
		for(i = 0; i < 1000; i++){
			waitForTick();
		}
		if(INTTriggered)
		{
			INTTriggered = 0;
			break;
		}
	}

	// ---=== TASK 1 ===---
	// write_usb_serial_blocking("TASK 1\n\r",8);
	// while(1)
	// {
	// 	double val = ADCRead();
	// 	char output[16];
	// 	sprintf(output, "ADC0: %8.4f\n\r", val);
	// 	write_usb_serial_blocking(output,16);

	// 	if(INTTriggered)
	// 	{
	// 		INTTriggered = 0;
	// 		break;
	// 	}

	// 	delayms(10);
	// }


	// ---=== TASK 2 ===---
	write_usb_serial_blocking("TASK 2\n\r",8);
	while(1)
	{
		DACSineWave(randDouble(1000),randDouble(1.5), 10000, &INTTriggered);
		if(INTTriggered)
		{
			INTTriggered = 0;
			break;
		}
	}

	// ---=== TASK 3 ===---
	write_usb_serial_blocking("TASK 3\n\r",8);
	while(1)
	{
		double Vin = ADCRead();
		DACSet(Vin);
		if(INTTriggered)
		{
			INTTriggered = 0;
			break;
		}
	}

	// ---=== TASK 4 ===---
	write_usb_serial_blocking("TASK 4\n\r",8);
	PWMSetup(25, 100);
	// Enable channel 1 in single edge mode
	PWMEnableChannel(1, SINGLE_EDGE);

	// Set the function Every_N to be called every 50 ms
	EVERY_N = 50;
	// Wait until the 5 cycles are done
	while(dutyCycle < 500){;}
	// Stop the function call
	EVERY_N = 0;
}

void Every_N()
{
	PWMStart(1, dutyCycle%100);
	dutyCycle++;
}