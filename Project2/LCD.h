void LCDClear(void);
void LCDHome(void);
void LCDTopLine(void);
void LCDBottomLine(void);
void LCDSetup(void);
void LCDWriteChar(char);
uint8_t * text2LCDBytes(char[], int, uint8_t[]);