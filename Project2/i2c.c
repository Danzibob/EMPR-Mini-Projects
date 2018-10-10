#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"

#include "i2c.h"

#define KEYPAD_I2C_ADDRESS   (0x21)
#define LCD_I2C_ADDRESS      (0x3B)
#define I2C_SCL_SDA_PORT     (0)
#define I2C_SDA_PIN          (0)
#define I2C_SCL_PIN          (1)
#define I2CDEV               LPC_I2C1

void setupI2C (void)
{
	// Create pin config for our I2C pins
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum     = PINSEL_FUNC_3;
	PinCfg.OpenDrain   = PINSEL_PINMODE_NORMAL;
	PinCfg.Pinmode     = PINSEL_PINMODE_PULLUP;
	PinCfg.Portnum     = I2C_SCL_SDA_PORT;

	// Apply config to SDA pin
	PinCfg.Pinnum      = I2C_SDA_PIN;
	PINSEL_ConfigPin(&PinCfg);

	// Apply config to SCL pin
	PinCfg.Pinnum      = I2C_SCL_PIN;
	PINSEL_ConfigPin(&PinCfg);

	// Configure I2C to run at 100000Hz
	I2C_Init(I2CDEV, 100000);

	// Enable the I2C device
	I2C_Cmd(I2CDEV, ENABLE);
}

Status sendBytes (char address, uint8_t data[])
{
	//I2C transmission config
	I2C_M_SETUP_Type i2c_m_setup;
	i2c_m_setup.sl_addr7bit = address;
	i2c_m_setup.tx_data = data;
	i2c_m_setup.tx_length = sizeof(data);
	i2c_m_setup.rx_data = NULL;
	i2c_m_setup.rx_length = 0;
	i2c_m_setup.retransmissions_max = 0;

	// Transfer data & return status
	return I2C_MasterTransferData(I2CDEV, &i2c_m_setup, I2C_TRANSFER_POLLING);
}