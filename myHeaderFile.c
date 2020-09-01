/*
 * myHeaderFile.c
 *
 *  Created on: 29-Jul-2020
 *      Author: ronit
 */
#include "myHeaderFile.h"

// Define "rENABLE_USART_INTERRUPT" to enable receive-interrupts from USART2
// Define "rENABLE_BUTTON_INTERRUPT" to enable interrupt from user button

void Initialize(void)
{
	DWT->CTRL |= (1<<0);			//	Enable DWT Cycle Count (DWTCYCCNT) Register.
									//	This register keeps count of no. of clock cycles completed after processor reset.

	RCC_DeInit();					// Reset clock to the default HSI configuration
	SystemCoreClockUpdate();		// Update system core clock variable to use the 16MHz value

	prvSetupHW();					// Setup MCU specific peripherals

	// Start Recording from Segger SysView
	SEGGER_SYSVIEW_Conf();
	SEGGER_SYSVIEW_Start();
}


void prvSetupHW(void)
{
	//	Use UART to communicate to PC
	prvSetupUART();

	// Use on-board led and button
	prvSetupNucleoButtonAndLED();
}

void prvSetupNucleoButtonAndLED(void)
{
	// Button PC13, LED PA5

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);

	GPIO_InitTypeDef LED_h, Button_h;			// Configuration structure for LED and Button pins

	// Configure LED pin
	LED_h.GPIO_Mode = GPIO_Mode_OUT;
	LED_h.GPIO_OType = GPIO_OType_PP;
	LED_h.GPIO_Pin = GPIO_Pin_5;
	LED_h.GPIO_PuPd = GPIO_PuPd_NOPULL;
	LED_h.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOA, &LED_h);

	// Configure Button Pin
	Button_h.GPIO_Mode = GPIO_Mode_IN;
	Button_h.GPIO_OType = GPIO_OType_PP;
	Button_h.GPIO_Pin = GPIO_Pin_13;
	Button_h.GPIO_PuPd = GPIO_PuPd_UP;
	Button_h.GPIO_Speed = GPIO_Low_Speed;
	GPIO_Init(GPIOC, &Button_h);

	// Configure Button Interrupt
#ifdef rENABLE_BUTTON_INTERRUPT
	// Enable clock for SYSCFG
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);

	// Configure EXTI settings in SYSCFG
	EXTI_InitTypeDef EXTI_h;		// EXTI configuration structure
	EXTI_h.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_h.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_h.EXTI_Line = EXTI_Line13;
	EXTI_h.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_h);

	//NVIC_SetPriority(EXTI15_10_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(EXTI15_10_IRQn, 6);

	// Configure NVIC
	NVIC_EnableIRQ(EXTI15_10_IRQn);
#endif
}

void prvSetupUART(void)
{
	//	PA2 is USART2 Tx, PA3 is USART2 Rx
	//	1. Enable UART2 and GPIOA Peripheral Clock
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

	// GPIO Configuration for UART pins
	GPIO_InitTypeDef UART_GPIO_Pins;
	memset(&UART_GPIO_Pins, 0, sizeof(UART_GPIO_Pins));			//	Reset all variables of the structure before initializing
	UART_GPIO_Pins.GPIO_Mode = GPIO_Mode_AF;
	UART_GPIO_Pins.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
	UART_GPIO_Pins.GPIO_PuPd = GPIO_PuPd_UP;

	// Initialize UART pins with above configuration.
	GPIO_Init(GPIOA, &UART_GPIO_Pins);
	// Configure the pins to be used in Alternate Function Mode
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);

	// Configuration for USART2 Peripheral
	USART_InitTypeDef UART2config;
	memset(&UART2config, 0, sizeof(UART2config));			//	Reset all variables of the structure before initializing
	UART2config.USART_BaudRate = 115200;
	UART2config.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	UART2config.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	UART2config.USART_Parity = USART_Parity_No;
	UART2config.USART_StopBits = USART_StopBits_1;
	UART2config.USART_WordLength = USART_WordLength_8b;
	USART_Init(USART2, &UART2config);


#ifdef rENABLE_USART_INTERRUPT
	// Enable the Interrupt
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);

	// Set priority in NVIC
	//NVIC_SetPriority(USART2_IRQn, configMAX_SYSCALL_INTERRUPT_PRIORITY);
	NVIC_SetPriority(USART2_IRQn, 6);

	// Enable interrupt reception on this irq num
	NVIC_EnableIRQ(USART2_IRQn);
#else
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
#endif

	// Enable the USART2 Peripheral
	USART_Cmd(USART2, ENABLE);
}

void printMsg(char *msg)
{
	for(unsigned int i = 0; i<strlen(msg); i++)
	{
		while( USART_GetFlagStatus(USART2, USART_FLAG_TXE) != SET );		// Wait for transmission complete
		USART_SendData(USART2, msg[i]);										// Send one character over UART
	}
}
