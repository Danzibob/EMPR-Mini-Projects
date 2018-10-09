#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>

#include "serial.c"

#define KEYPAD_I2C_ADDRESS   (0x21)
#define LCD_I2C_ADDRESS      (0x3B)
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

	uint8_t data_out[] = {0x00,0x00};
	uint8_t data_in = 0x00;

	uint8_t setupBytes[] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02};
	Status i2c_status;
	//I2C transmission setup
	I2C_M_SETUP_Type i2c_m_setup;
	i2c_m_setup.sl_addr7bit = LCD_I2C_ADDRESS;
	i2c_m_setup.tx_data = setupBytes;
	i2c_m_setup.tx_length = sizeof(setupBytes);
	i2c_m_setup.rx_data = NULL;
	i2c_m_setup.rx_length = 0;
	i2c_m_setup.retransmissions_max = 1;

	i2c_status = I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
	displayNibble(0);
	delayms(1000);

	i2c_m_setup.tx_data = data_out;
	i2c_m_setup.tx_length = sizeof(data_out);

	data_out[0] = 0x80;
	data_out[1] = 0x01;
	i2c_status = I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
	displayNibble(1);
	delayms(1000);

	data_out[0] = 0x80;
	data_out[1] = 0x00;
	i2c_status = I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
	displayNibble(2);
	delayms(1000);

	char helloWorld[] = {0x40, 0xC8, 0xE9, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0};
	i2c_m_setup.tx_data = helloWorld;
	i2c_m_setup.tx_length = sizeof(helloWorld);
	i2c_status = I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
	displayNibble(3);
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
	delayms(100);
}