#define KEYPAD_I2C_ADDRESS   (0x21)

uint8_t readKeypad(void);
char getKey(void);
Status writeKeypad(int col_num);

char keypadKeys[4][4] = {
	{'D','#','0','*'},
	{'C','9','8','7'},
	{'B','6','5','4'},
	{'A','3','2','1'}
};

Status writeKeypad(int col_num)
{
	uint8_t byte[1] = {0xff};
	uint8_t row = 0x01 << col_num;
	// Turn off only the nth bit
	byte[0] = byte[0] & (~row);
	Status s = sendBytes(KEYPAD_I2C_ADDRESS, byte, 1);
	return s;
}

uint8_t readKeypad(void)
{
	uint8_t data[1];
	readBytes(KEYPAD_I2C_ADDRESS, data, 1);
	// Convert read byte to one-hot column representation
	return (~data[0] & 0xf0) >> 4;
}

char getKey(void)
{
	int row;
	for(row = 0; row < 4; row++)
	{
		writeKeypad(row);
		delayms(1);
		uint8_t keypadBits = readKeypad();
		int val = oneHot2Bin(keypadBits);
		if(val != -1) // -1 indicates no keys pressed
		{
			// Breaks out of the loop to return - no key rollover*
			return keypadKeys[row][val];
		}
		delayms(1);
	}
	// ' ' indicates no key was found to be pressed
	return ' ';
}