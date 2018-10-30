void initPWM(){
	PINSEL_CFG_Type PinCfg;
	PinCfg.Funcnum = 1;
	PinCfg.OpenDrain = 0;
	PinCfg.Pinmode = 0;
	PinCfg.Portnum = 2;
	PinCfg.Pinnum = 1;
	//Real pin 26 to 21
	PINSEL_ConfigPin(&PinCfg);


	PWM_ChannelCmd(LPC_PWM1, 2, ENABLE);

	PWM_ChannelConfig(LPC_PWM1, 2, PWM_CHANNEL_SINGLE_EDGE);

	PWM_Cmd(LPC_PWM1, ENABLE);


	PWM_MATCHCFG_Type matchCfg;
	matchCfg.IntOnMatch = DISABLE;
	matchCfg.MatchChannel = 2;
	matchCfg.IntOnMatch = DISABLE;
	matchCfg.StopOnMatch = DISABLE;
	PWM_ConfigMatch(LPC_PWM1, &matchCfg);

	PWM_TIMERCFG_Type tmrCfg;
	PWM_ConfigStructInit(PWM_MODE_TIMER, &tmrCfg);

	PWM_Init(LPC_PWM1, PWM_MODE_TIMER, &tmrCfg);

	PWM_MatchUpdate(LPC_PWM1, 2, 2, PWM_MATCH_UPDATE_NOW);
}
