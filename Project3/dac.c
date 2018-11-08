#include "dac.h"
#define AOUT 26;
#define PI 3.14159265
void DACSetup(void)
{
	// Configure pins
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum     = 2;
	PinCfg.Portnum     = 0;	
	PinCfg.Pinmode     = 0;
	PinCfg.OpenDrain = 0;
	// Apply config
	PinCfg.Pinnum      = AOUT;
	PINSEL_ConfigPin(&PinCfg);

	// Initialise DAC
	DAC_Init(LPC_DAC);
}

void DACSet(double voltage)
{
	// Update value to DAC
	DAC_UpdateValue(LPC_DAC, DACVoltageToValue(voltage));
}

void DACSineWave(double Period, double Amplitude, double time, int *INTT)
{
	uint32_t ticks = 0;
	while(ticks < time){
		double xScaled = (ticks/Period) * (2*PI);
		double y = (sin(xScaled) + 1) * Amplitude;
		DACSet(y);
		ticks++;
		waitForTick();
		if(*INTT == 1) break;
	}
}

int DACVoltageToValue(double V)
{
	int val = V / 1.667 * 512;
	if(val < 0)
	{
		val = 0;
	} else if (val > 1023)
	{
		val = 1023;
	}
	return val;
}