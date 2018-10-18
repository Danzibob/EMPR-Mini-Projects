#define LCD_I2C_ADDRESS      (0x3B)
#define LCD_WAIT_MS          (10)

#include "LCD.h"
#include "helpers.c"

void LCDTopLine(void)
{
	uint8_t topLine[] = {0x00, 0x80};
	sendBytes(LCD_I2C_ADDRESS, topLine, 2);
	delayms(LCD_WAIT_MS);
}

void LCDBottomLine(void)
{
	uint8_t topLine[] = {0x00, 0xC0};
	sendBytes(LCD_I2C_ADDRESS, topLine, 2);
	delayms(LCD_WAIT_MS);
}

void LCDHome(void)
{
	uint8_t topLine[] = {0x00, 0x01};
	sendBytes(LCD_I2C_ADDRESS, topLine, 2);
	delayms(LCD_WAIT_MS);
}

void LCDWriteChar(char C)
{
	char ASCII[2] = {C};
	uint8_t letter[2];
	text2LCDBytes(ASCII, 2, letter);
	sendBytes(LCD_I2C_ADDRESS, letter, 2);
	delayms(LCD_WAIT_MS);
}

void LCDClear(void)
{
	LCDTopLine();
	delayms(LCD_WAIT_MS);
	uint8_t clear[] = {0x40, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0, 0xA0};
	sendBytes(LCD_I2C_ADDRESS, clear, 17);
	delayms(LCD_WAIT_MS);
	LCDBottomLine();
	sendBytes(LCD_I2C_ADDRESS, clear, 17);
	delayms(LCD_WAIT_MS);
}

void LCDSetup(void)
{
	uint8_t setupBytes[] = {0x00,0x34,0x0c,0x06,0x35,0x04,0x10,0x42,0x9f,0x34,0x02};
	sendBytes(LCD_I2C_ADDRESS, setupBytes, 11);
	delayms(200);
}

uint8_t * text2LCDBytes(char string[], int len, uint8_t out[])
{
	out[0] = 0x40;
	int i;
	for(i = 0; i < len-1; i++)
	{
		// Check if the character is a letter (UPPER or lower)
		if((' ' <= string[i] && string[i] <= 'Z') || ('a' <= string[i] && string[i] <= 'z'))
		{
			out[i+1] = string[i] + 128;
		} 
		else
		{
			out[i+1] = string[i];
		}
	}
	return out;
}