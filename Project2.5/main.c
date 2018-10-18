#include "LPC17xx.h"
#include "lpc_types.h"
#include "lpc17xx_pinsel.h"
#include "lpc17xx_i2c.h"
#include "lpc17xx_gpio.h"
#include <stdio.h>
#include <stdlib.h>

#include "serial.c"
#include "i2c.c"
#include "LCD.c"
#include "Keypad.c"

Status i2c_status;

void main (void)
{
	// Initialize stuff
	SysTick_Config(SystemCoreClock / 1000);
	serial_init();
	setupI2C();
	LCDSetup();
	LCDClear();
	LCDTopLine();

	// Set up variables
	char lastKey = ' ';
	int cursor = 0;

	char txt0[16] = "                ";
	char txt1[16] = "                ";
	char *txtArgs[] = {txt0, txt1};
	int currentArg = 0;
	float arg0, arg1, result;
	char operator = 0x00;

	// Main program loop
	while(1)
	{
		// Read and display characters from keypad
		char key = getKey();
		if(lastKey != key) 
		{
			lastKey = key;
			if(key != ' ')
			{
				// --== Key pressed logic ==--

				// if key is a number, add it to the current operand
				if(('0' <= key && key <= '9') || key == '*')
				{
					// if the argument hit max length, error out
					if(cursor == 15)
					{
						/* ERROR OUT HERE */
						delayms(2000);
						/* RESET HERE */
						continue;
					}

					if(key == '*') key = '.';

					txtArgs[currentArg][cursor] = key;
					LCDWriteChar(key);
					cursor++;
				} else 
				// if the key is an operator
				if('A' <= key && key <= 'D')
				{
					if(currentArg == 0)
					{
						currentArg = 1;
						cursor = 0;
						operator = key;

						// Write arg0 and operator to bottom line
						LCDClear();
						LCDBottomLine();
						uint8_t LCDData[16];
						text2LCDBytes(txt0, 16, LCDData);
						sendBytes(LCD_I2C_ADDRESS, LCDData, 16);
						LCDWriteChar(key);
						LCDTopLine();
					}
				} else 
				if(key == '#')
				{
					arg0 = atof(txt0);
					arg1 = atof(txt1);
					// Carry out operation
					switch (operator){
						case 'A': // Addition
							result = arg0 + arg1;
							break;
						case 'B': // Subtraction
							result = arg0 - arg1;
							break;
						case 'C': // Multiplication
							result = arg0 * arg1;
							break;
						case 'D': // Division
							result = arg0 / arg1;
							break;
						default:
							/* ERROR OUT HERE */
						break;
					}
					// Write result to display
					LCDClear();
					LCDBottomLine();
					char textResult[16];
					memset(textResult, '\0', 16);
					sprintf(textResult, "%-15.4f", result);
					uint8_t LCDResult[16];
					text2LCDBytes(textResult, 16, LCDResult);
					sendBytes(LCD_I2C_ADDRESS, LCDResult, 16);

					// Set arguments
					memset(txt0, '\0', 16);
					memset(txt1, '\0', 16);
					strcpy(txt0, textResult);
					currentArg = 0;
					cursor = 0;
					// Reset text entry
					LCDTopLine();
				}

			}
		}
	}

}
