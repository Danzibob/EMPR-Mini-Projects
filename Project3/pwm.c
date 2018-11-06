#define SINGLE_EDGE 0
#define DOUBLE_EDGE 1

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