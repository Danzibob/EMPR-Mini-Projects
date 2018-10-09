#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"

#define KEYPAD_I2C_ADDRESS   (0x42)
#define I2C_SCL_SDA_PORT     (0)
#define I2C_SDA_PIN          (0)
#define I2C_SCL_PIN          (1)
#define I2CDEV               LPC_I2C1

// Define our pin numbers
#define LED1 18
#define LED2 20
#define LED3 21
#define LED4 23
int LEDs[4] = {1<<LED1, 1<<LED2, 1<<LED3, 1<<LED4};
int allLEDs= (1<<LED1)|(1<<LED2)|(1<<LED3)|(1<<LED4);

void setupI2C (void);
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

	GPIO_SetDir(1, 0xFFFFFFFF, 1);

	displayNibble(0x1);

	uint8_t data_out = 0xEF;
	uint8_t data_in = 0x00;

	I2C_M_SETUP_Type i2c_m_setup;

	i2c_m_setup.sl_addr7bit = KEYPAD_I2C_ADDRESS >> 1;
	i2c_m_setup.tx_data = &data_out;
	i2c_m_setup.tx_length = sizeof(data_out);
	i2c_m_setup.rx_data = &data_in;
	i2c_m_setup.rx_length = sizeof(data_in);
	i2c_m_setup.retransmissions_max = 3;

	displayNibble(0x2);

	int num_devices = 0;
	int i;
	for(i=0; i < 1<<7; i++)
	{
		i2c_m_setup.sl_addr7bit =  i;
		Status i2c_status = I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
		if(i2c_status != 0)
		{
			char outStr[7];
			sprintf(outStr, "0x%02X, ", i);
			write_usb_serial_blocking(outStr,7);
			num_devices++;
		}
	}
	char outStr2[37];
	sprintf(outStr2, "\n\r%3d devices connected to i2c bus\n\r", num_devices);
	write_usb_serial_blocking(outStr2,37);
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


void setupI2C (void)
{
	displayNibble(0x8);
	/* Configure Pins */
	PINSEL_CFG_Type PinCfg;

	PinCfg.Funcnum     = PINSEL_FUNC_3;
	PinCfg.OpenDrain   = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode     = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum     = I2C_SCL_SDA_PORT;

	PinCfg.Pinnum      = I2C_SDA_PIN;
	PINSEL_ConfigPin(&PinCfg);

	displayNibble(0x9);

	PinCfg.Pinnum      = I2C_SCL_PIN;
	PINSEL_ConfigPin(&PinCfg);

	// Configure I2C to run at 100000Hz
	I2C_Init(I2CDEV, 100000);

	displayNibble(0xA);

	// Enable the I2C device
	I2C_Cmd(I2CDEV, ENABLE);
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
}