#define SINGLE_EDGE 0
#define DOUBLE_EDGE 1

void initAllPWM(){

	// Configure pins
	LPC_PINCON->PINSEL4 = 0b01010101;

	// Timer Control Register
	/*
	Bit 0 : When 1 both PWM Timer counter and PWM Prescale counter are enabled. When 0 both are disabled.
	Bit 1 : When set to 1 it will reset Reset both Timer and Prescale counter of PWM block (at next edge of PCLK).
	Bit 3 : Enables PWM mode i.e the PWM outputs.
	Other Bits : Reserved.
	*/
	LPC_PWM1->TCR = 0x08;

	// Prescale Register
	// The Timer Counter(TC) will increment every PWMPR+1 Peripheral Clock Cycles (PCLK).
	// 25 PCLK cycles to increment TC by 1 i.e. 1 Micro-second
	LPC_PWM1->PR  = (25 - 1);
	
	// Match Control Register
	/*
	Bit 0 : Interrupt on PWM1MR0 Match
		If set to 1 then it will generate an Interrupt else disable if set to 0.
	Bit 1 : Reset on PWM1MR0 Match
		If set to 1 it will reset the PWM Timer Counter i.e. PWM1TC else disabled if set to 0.
	Bit 2 : Stop on PWM1MR0 Match
		If this bit is set 1 then both PWM1TC and PWM1PC will be stopped & disable the Counters.
	Bit 3-5 : PWM1MR1
	Bit 6-8 : PWM1MR2
	...
	*/
	// But for some reason it has to be 2.¯\_(ツ)_/¯
	LPC_PWM1->MCR = 2;

    // Match Registers - manage duty cycles
    // MR0: Period; MRX, duty cycle for that PWM pin
	LPC_PWM1->MR0 = 1000;
	// Set duty cycle for all pins
	LPC_PWM1->MR1 = 800;
	LPC_PWM1->MR2 = 500;
	LPC_PWM1->MR3 = 300;
	LPC_PWM1->MR4 = 100;

	// Load Enable Register
	/*
	Bit ‘x’ in PWM1LER corresponds to match Register MRx
	Loads the new Match Register value
	*/
	LPC_PWM1->LER = 0x1F;

	// Enable Timer Control Register
	/*
	Bit 0 : When 1 both PWM Timer counter and PWM Prescale counter are enabled. When 0 both are disabled.
	Bit 1 : When set to 1 it will reset Reset both Timer and Prescale counter of PWM block (at next edge of PCLK).
	Bit 2 : Enables PWM mode i.e the PWM outputs.
	*/
	LPC_PWM1->TCR = 0x01;

	// PWM Control Register
	/*
	1) Bits 2 to 6 are used to select between Single or Double Edge mode for PWM 2,3,4,5,6 outputs. 
	   If Bit 2 is set to 1 then PWM1.2(i.e the one corresponding to PWM1MR2) output is double edged 
	   else if set 0 then its Single Edged. Similarly Bit x for PWM1.x correspondingly.
	2) Bits 9 to 14 are used to Enable/Disable PWM outputs. 
	   If Bit 9is set to 1 then PWM1.1 output is enabled,
	   else disabled if set to 0. Similarly remaining bits for PWM1.x correspondingly. 
	*/
	LPC_PWM1->PCR = 0x1e00;
}

void PWMSetup(int resolution, int period)
{
	// Enable PWM
	LPC_PWM1->TCR = 0x08;

	// This has to be 2 for some reason
	LPC_PWM1->MCR = 2;

	// Set us resolution (24 -> 1us)
	LPC_PWM1->PR  = resolution - 1;

	// Set period of PWM cycle (if PR = 24 and MR0 = 1000 then Period = 1ms)
	LPC_PWM1->MR0 = period;

	// Load this new MR register val
	LPC_PWM1->LER = 0x01;

	// Enable timer and prescaler counters
	LPC_PWM1->TCR = 0x05;
}

void PWMEnableChannel(uint8_t channel, int edge_type)
{
	// Set the pin mode
	LPC_PINCON->PINSEL4 |= (1 << (channel-1)*2);

	// Ignore edge type for unconfigurable PWM channels
	if(channel < 2){
		edge_type = 0;
	}
	// Set single/double edge and enable / disable respectively
	LPC_PWM1->PCR = (edge_type << channel) | (1 << (channel + 8));
}

void PWMStart(uint8_t channel, int duty){

	// Set duty cycle for given channel
	switch (channel){
		case 1:
			LPC_PWM1->MR1 = duty;
			break;
		case 2:
			LPC_PWM1->MR2 = duty;
			break;
		case 3:
			LPC_PWM1->MR3 = duty;
			break;
		case 4:
			LPC_PWM1->MR4 = duty;
			break;
		default:
			break;
	}
	// Update value with latch
	LPC_PWM1->LER = (1 << channel);
}