#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "serial.c"
#include "i2c.c"
#include "LCD.c"
#include "Keypad.c"
#include "adc.c"
#include "dac.c"

Status i2c_status;

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

	double x = 0;

	// Do Stuff
	ADCSetup();

	delayms(20);

	// while(1)
	// {
	// 	double val = ADCRead();
	// 	char output[17];
	// 	sprintf(output, "ADC0: %8.4f\n\r", val);
	// 	write_usb_serial_blocking(output,17);
	// 	delayms(10);
	// }

	DACSetup();

	while(1)
	{
		DACSineWave(randDouble(1000),randDouble(1.5), 2000);
	}
}