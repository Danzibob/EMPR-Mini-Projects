#include "adc.h"
#define ADC0_0 23

void ADCSetup(void)
{
	// Configure pins
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum     = 1;
	PinCfg.Portnum     = 0;	
	PinCfg.Pinmode     = 0;
	PinCfg.OpenDrain   = 0;
	// Apply config
	PinCfg.Pinnum      = ADC0_0;
	PINSEL_ConfigPin(&PinCfg);

	// initialize ADC
	ADC_Init(LPC_ADC, 200000);

	// Initialize interrupts here

	// Enable the channel
	ADC_ChannelCmd(LPC_ADC, 0, ENABLE);
	ADC_BurstCmd(LPC_ADC,ENABLE);
}

double ADCRead(void)
{
	return ADCToVoltage(ADC_ChannelGetData(LPC_ADC, 0));
}

double ADCToVoltage(uint32_t adc){
  return ((double) adc)/((double) 4096) * 3;
}