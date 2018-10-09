# Setting Pin Modes
- PINSEL0 Controls function of lower part of port 0
- PINSEL1 Controls function of upper part of port 0
- 2&3 control port 1, etc

# Serial IO
- CPU uses UART, this is bridges to the mini-USB on the MBED board
- TXD0 and RXD0 are P0.2 & P0.3
- The UART for these pins are on function code "01"

*wooooooo, driver functions...*

# Keyboard and LCD
- Both on I2C Bus
- IO Board uses I2C1
  - Mode 3 of pins P0.0 & P0.1
- *presumably both* have driver chips sat on the I2C bus

## LCD
- 2 rows of 16 chars (each 5x7px, or 5x8 for user-defined)
- uses **PCF2119X** driver chip
  - 2 Registers: *Instruction* register (also holds addresses) and *Data* register
- Only these 2 regs can be accessed via the I2C bus
- I2C message must start with a control byte - determines reg to write to
  - Bit 6 = Reg Sel -> 0 = Instruction reg, 1 = Data Reg
  - Bit 7 = ¬(Last Control Byte) | (Another control byte follows this data)
   - Setting this indicates that bits 0-6 contain a DDRAM address
  - **REMEMBER**: Bit 7 is the MSB, not the 7th bit from the right
- Can read a *busy* flag, checks if chip is ready for a write
- Datasheet /w function table on VLE

## Keypad
- PCF8574 IC
- 16 keys, 8 IO lines
- Scan through Y (make low) and find when X is brought low
  - So Yx4 are write and Xx4 are read
- The IC is apparently garbage and does no work for us
