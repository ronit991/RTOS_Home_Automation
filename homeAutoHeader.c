#include "homeAutoHeader.h"

// 14 Pins Available for Switches
uint16_t ApplSwitchPins[14] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_2, GPIO_Pin_3, GPIO_Pin_4, GPIO_Pin_5, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};
// 13 Pins Available for Relays
uint16_t ApplRelayPins[13] = {GPIO_Pin_0, GPIO_Pin_1, GPIO_Pin_4, GPIO_Pin_6, GPIO_Pin_7, GPIO_Pin_8, GPIO_Pin_9, GPIO_Pin_10, GPIO_Pin_11, GPIO_Pin_12, GPIO_Pin_13, GPIO_Pin_14, GPIO_Pin_15};


/**********************************************************************************************************************
 * @function	-	prv_Configure_Appliance_Switch_GPIO_Pins()
 *
 * @description	-	This function configures all pins (available on nucleo board) of GPIO Port B as Input Pull-Up Pins.
 *					These Pins are used as switch inputs of different home appliances. All of these pins deliver an
 *					interrupt to the MCU to wake their corresponding tasks.
 *********************************************************************************************************************/
static void prv_Configure_Appliance_Switch_GPIO_Pins(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

	GPIO_InitTypeDef Button_h;			// Configuration structure for Button pins

	uint16_t ApplSwitchGPIOPins = 0;
	for(int i = 0; i < TotalSwitchPins; i++)
	{
		ApplSwitchGPIOPins |= ApplSwitchPins[i];
	}
	// Configure Button Pin
	Button_h.GPIO_Mode = GPIO_Mode_IN;
	Button_h.GPIO_OType = GPIO_OType_PP;
	Button_h.GPIO_Pin = ApplSwitchGPIOPins;
	Button_h.GPIO_PuPd = GPIO_PuPd_UP;
	Button_h.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOB, &Button_h);


	// Configure Button Interrupts
	// Enable clock for SYSCFG
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource1);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource4);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource5);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource6);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource7);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource8);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource9);
	// PB10/11 Pins are not present on our nucleo board
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource12);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource13);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource14);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource15);


	// Configure EXTI settings in SYSCFG
	EXTI_InitTypeDef EXTI_h;		// EXTI configuration structure
	EXTI_h.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_h.EXTI_Trigger = EXTI_Trigger_Rising;
	//EXTI_h.EXTI_Line = EXTI_Line13;
	EXTI_h.EXTI_LineCmd = ENABLE;
	//EXTI_Init(&EXTI_h);
	for(int i = 0; i < TotalSwitchPins; i++)
	{
		EXTI_h.EXTI_Line = ApplSwitchPins[i];
		EXTI_Init(&EXTI_h);
	}

	// Set Priority
	NVIC_SetPriority(EXTI0_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI1_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI2_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI3_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI4_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI9_5_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
	NVIC_SetPriority(EXTI15_10_IRQn, configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY+1);
/*
	NVIC_SetPriority(EXTI0_IRQn, 6);
	NVIC_SetPriority(EXTI1_IRQn, 6);
	NVIC_SetPriority(EXTI2_IRQn, 6);
	NVIC_SetPriority(EXTI3_IRQn, 6);
	NVIC_SetPriority(EXTI4_IRQn, 6);
	NVIC_SetPriority(EXTI9_5_IRQn, 6);
	NVIC_SetPriority(EXTI15_10_IRQn, 6);
*/
	// Enable Interrupt Reception
	NVIC_EnableIRQ(EXTI0_IRQn);
	NVIC_EnableIRQ(EXTI1_IRQn);
	NVIC_EnableIRQ(EXTI2_IRQn);
	NVIC_EnableIRQ(EXTI3_IRQn);
	NVIC_EnableIRQ(EXTI4_IRQn);
	NVIC_EnableIRQ(EXTI9_5_IRQn);
	NVIC_EnableIRQ(EXTI15_10_IRQn);
}



/**********************************************************************************************************************
 * @function	-	prv_Configure_Appliance_Relay_GPIO_Pins()
 *
 * @description	-	This function configures all available GPIO Port A pins as Output pins. These pins are used to
 * 					drive the relays connected to home appliances.
 *********************************************************************************************************************/
static void prv_Configure_Appliance_Relay_GPIO_Pins(void)
{
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	GPIO_InitTypeDef Button_h;			// Configuration structure for Button pins

	uint16_t ApplRelayGPIOPins = ApplRelayPins[0] | ApplRelayPins[1] | ApplRelayPins[2] | ApplRelayPins[3] | ApplRelayPins[4] | ApplRelayPins[5] | ApplRelayPins[6] | ApplRelayPins[7] | ApplRelayPins[8] | ApplRelayPins[9] | ApplRelayPins[10] | ApplRelayPins[11] | ApplRelayPins[12];
	// Configure Button Pin
	Button_h.GPIO_Mode = GPIO_Mode_OUT;
	Button_h.GPIO_OType = GPIO_OType_PP;
	Button_h.GPIO_Pin = ApplRelayGPIOPins;
	Button_h.GPIO_PuPd = GPIO_PuPd_NOPULL;
	Button_h.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA, &Button_h);
}



/**********************************************************************************************************************
 * @function	-	Initialize_Home_Automation_Things()
 *
 * @description	-	This API calls all the configuration functions used for GPIO and other operations used in our
 * 					project.
 *********************************************************************************************************************/
void Initialize_Home_Automation_Things(void)
{
	prv_Configure_Appliance_Relay_GPIO_Pins();
	prv_Configure_Appliance_Switch_GPIO_Pins();
}
