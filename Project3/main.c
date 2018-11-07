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

#define GPIO_

Status i2c_status;
int dutyCycle;

void EINT3_IRQHandler(void)
{
	// Clear the interrupt on the pin
	LPC_GPIOINT->IO0IntClr = (1 << 9);
	// Do stuff
	resetInterrupt();
	// Blinky Blink
	if (LPC_GPIO1->FIOPIN & (1 << 18))
    {
        LPC_GPIO1->FIOCLR |= (1 << 18);
    }
    else
    {
        LPC_GPIO1->FIOSET |= (1 << 18);
    }
}

void main (void)
{
	// // Initialize SysTick interrupts every ms
	// SysTick_Config(SystemCoreClock / 1000);

	// // Initialize stuff
	// GPIO_SetDir(2, 0xFFFFFFFF, 1);
	// serial_init();
	// setupI2C();
	// LCDSetup();
	// LCDClear();
	// ADCSetup();
	// DACSetup();

	// Set LED1 as an output
	LPC_GPIO1->FIODIR |= (1 << 21);

	// Set up keypad interrupts

	// Set P0.9 as input
	LPC_GPIO0->FIODIR &= ~(1 << 9);
	// Enable interrupt on pin 0.9
	LPC_GPIOINT->IO0IntEnF |= (1 << 9);
	// Enable GPIO interrupts
	NVIC_EnableIRQ(EINT3_IRQn);
	// Enable interrupts globally
	__enable_irq();
	// Reset the keypad's internal interrupts
	resetInterrupt();


	// ---=== TASK 1 ===---

	// while(1)
	// {
	// 	double val = ADCRead();
	// 	char output[17];
	// 	sprintf(output, "ADC0: %8.4f\n\r", val);
	// 	write_usb_serial_blocking(output,17);
	// 	delayms(10);
	// }


	// ---=== TASK 2 ===---

	// while(1)
	// {
	// 	DACSineWave(randDouble(1000),randDouble(1.5), 10000);
	// }

	// ---=== TASK 3 ===---
	// while(1)
	// {
	// 	double Vin = ADCRead();
	// 	DACSet(Vin);
	// }

	// ---=== TASK 4 ===---
	// PWMSetup(25, 100);
	// // Enable channel 1 in single edge mode
	// PWMEnableChannel(1, SINGLE_EDGE);

	// // Set the function Every_N to be called every 50 ms
	// EVERY_N = 50;
	// // Wait until the 5 cycles are done
	// while(dutyCycle < 500){;}
	// // Stop the function call
	// EVERY_N = 0;
}

void Every_N()
{
	PWMStart(1, dutyCycle%100);
	dutyCycle++;
}