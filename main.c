#include "myHeaderFile.h"
#include "homeAutoHeader.h"

/* ********************************************** Flags & Misc. Alias ********************************************** */
#define CMD_ON		0x01
#define CMD_OFF		0x00

#define Auto		True
#define Manual		False

#define Active		True
#define NotActive	False


/* ************************************************ Appliance IDs ************************************************** */
#define Appl00_Fan_Cooler			0x00
#define Appl01_Light1				0x01
#define Appl02_Light2				0x02
#define Appl03_						0x03
#define Appl04_						0x04
#define Appl05_						0x05
#define Appl06_						0x06
#define Appl07_						0x07
#define Appl08_						0x08
#define Appl09_						0x09
#define Appl10_Curtains				0x0A
#define Appl11_NightLights			0x0B
#define Appl12_StaircaseLights		0x0C
#define Appl13_Doorbell				0x0D

uint8_t ApplianceStates[14] = {CMD_OFF};


/* ************************************************* Task Handles ************************************************** */
TaskHandle_t xTH_Test;
// Continuous
TaskHandle_t xCT1_H_SyncWithServer;

// Occasional
TaskHandle_t xOT1_H_FanSpeedControl;
TaskHandle_t xOT2_H_ApplianceControl;

// Rare -> Event Triggered
TaskHandle_t xRT1_H_Doorbell;
TaskHandle_t xRT2_H_StaircaseLights;
TaskHandle_t xRT3_H_NightLight;
TaskHandle_t xRT4_H_Curtains;


/* ********************************************* Interrupt Functions *********************************************** */
void FireAndTheftAlarm(void);
void vToggleAppliance( uint8_t ApplianceID, BaseType_t* pxHigherPriorityTaskWoken);


/* ************************************************ Task Functions ************************************************* */
void vT_Test(void* param);
// Continuous
void vCT1_SyncWithServer(void* param);

// Occasional
void vOT1_FanSpeedControl(void* param);
void vOT2_ApplianceControl(void* param);

// Rare -> Event Triggered
void vRT1_Doorbell(void* param);
void vRT2_StaircaseLights(void* param);
void vRT3_NightLight(void* param);
void vRT4_Curtains(void* param);


/* ************************************************ User Functions ************************************************* */
void blinkLED(void);


/* ************************************************** User Types *************************************************** */
typedef struct
{
	uint8_t ApplID;
	uint8_t ApplCmd;
}xApplianceCmdType;



/* ************************************************* Global Space ************************************************** */
char message[250];

// Queue Handles
QueueHandle_t xApplianceCmdQueue;

// Fan/Cooler Control
uint8_t FanCoolerControl = Auto;
#define FanOrCoolerIsOn ApplianceStates[Appl00_Fan_Cooler]

// Commands waiting to be processed
#define NoOfCommandsWaiting uxQueueMessagesWaiting(xApplianceCmdQueue)

// Doorbell State
uint8_t DoorbellState = NotActive;
#define The_Guest_Is_Still_There				True				//	<<<<<<<<<<<<<<	This needs to be updated.
#define The_Gate_Was_Opened_While_Waiting		!DoorbellState

// Home Status
uint8_t HouseIsEmpty = False;


/* ***************************************************** main ****************************************************** */
int main(void)
{
	Initialize();
	Initialize_Home_Automation_Things();

	blinkLED();

	// Create the tasks
	//xTaskCreate(vT_Test,	"T1: Test Task",  500, NULL, 3, &xTH_Test);
	// Create the tasks
	xTaskCreate(vCT1_SyncWithServer,	"CT1: Sync Data with Server",  500, NULL, 1, &xCT1_H_SyncWithServer);

	xTaskCreate(vOT1_FanSpeedControl,	"OT1: Fan/Cooler Speed Control",			500, NULL, 2, &xOT1_H_FanSpeedControl);
	xTaskCreate(vOT2_ApplianceControl,	"OT2: Switch on/off home appliances",		500, NULL, 2, &xOT2_H_ApplianceControl);

	xTaskCreate(vRT1_Doorbell,			"RT1: Ring door-bell when someone arrives",	500, NULL, 3, &xRT1_H_Doorbell);
	xTaskCreate(vRT2_StaircaseLights,	"RT2: Turn on/off Staircase Lights",		500, NULL, 3, &xRT2_H_StaircaseLights);
	xTaskCreate(vRT3_NightLight,		"RT3: Turn on/off Night Lamps",				500, NULL, 3, &xRT3_H_NightLight);
	xTaskCreate(vRT4_Curtains,			"RT4: Open Window Curtain at Sunrise",		500, NULL, 3, &xRT4_H_Curtains);


	// Create Queues
	xApplianceCmdQueue = xQueueCreate(5, sizeof(xApplianceCmdType));

	// Start the Scheduler
	printMsg("\n\r Starting scheduler... \n\r");
	vTaskStartScheduler();

	while(1);
}



/* ******************************************** Task Function Definition ******************************************* */
void vT_Test(void* param)
{
	while(1)
	{
	}
}



void vCT1_SyncWithServer(void* param)
{
	while(1)
	{
		/* 1. Fetch pending command requests from the server
		 * 2. Execute the commands
		 * 3. Collect sensor and appliance-state data
		 * 4. Send all this to the server
		 */
	}
}



void vOT1_FanSpeedControl(void* param)
{
	while(1)
	{
		// Get into blocked state and wait for a notification
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
/*
		// Unblock on a notif

		//
		printMsg("\n\r OT1 \n\r");
		while( FanOrCoolerIsOn )
		{
			if( FanCoolerControl == Auto)
			{
			// Read Humidity and Temperature
			// Adjust speed
			// Wait for some time
			}
			else
			{
				break;
			}
			vTaskDelay(pdMS_TO_TICKS(10000));
		}
		*/
	}
}



void vOT2_ApplianceControl(void* param)
{
	xApplianceCmdType CurrentCMD;
	while(1)
	{
		// Get into blocked state and wait for a notification
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);

		// Unblock on a notif

		// Fetch command
		while( NoOfCommandsWaiting > 0)
		{
			xQueueReceive( xApplianceCmdQueue, &CurrentCMD, 0);

			sprintf(message, "\n\r Got command %d for Appliance %d.", CurrentCMD.ApplCmd, CurrentCMD.ApplID);
			printMsg(message);

			ApplianceStates[CurrentCMD.ApplID] = CurrentCMD.ApplCmd;

			// Switch on/off an appliance
			switch(CurrentCMD.ApplCmd)
			{
				case CMD_ON:	/*
								if(CurrentCMD.ApplID == Appl00_Fan_Cooler)
								{
									// send a notification to Fan/Cooler speed control task
									xTaskNotify(xOT1_H_FanSpeedControl, 0, eNoAction);
								}*/
								GPIO_WriteBit(ApplianceRelayGPIOPort, ApplRelayPins[CurrentCMD.ApplID], Bit_SET);
								break;
				case CMD_OFF:	GPIO_WriteBit(ApplianceRelayGPIOPort, ApplRelayPins[CurrentCMD.ApplID], Bit_RESET);
								break;
				default:		break;
			}
		}

		printMsg(" All done.\n\r");
	}
}



void vRT1_Doorbell(void* param)
{
	while(1)
	{
		// Get into Blocked State
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		// Unblock on an Interrupt

		if(HouseIsEmpty)
		{
			// Play a sound to notify the guest that there is no one in the house.
		}
		else
		{
			do
			{	// Ring the doorbell

				// Wait for 10 seconds
				vTaskDelay(pdMS_TO_TICKS(10000));

				if( The_Gate_Was_Opened_While_Waiting )
				{
					// Change the state to "NotActive"
					DoorbellState = NotActive;
					// Block the task
					break;
				}
				// Else, Ring the bell again
			}while( The_Guest_Is_Still_There );
		}
	}
}



void vRT2_StaircaseLights(void* param)
{
	while(1)
	{
		// Get into Blocked State
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		// Unblock on an Interrupt
		printMsg("\n\r RT2 \n\r");
		// Switch on the lights
		// Check if the stairs are empty or not
		//		if empty, switch off the lights and get into blocked state
	}
}



void vRT3_NightLight(void* param)
{
	while(1)
	{
		// Get into Blocked State
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		// Unblock on an Interrupt
		printMsg("\n\r RT3 \n\r");
		// Turn on/off night light
	}
}



void vRT4_Curtains(void* param)
{
	while(1)
	{
		// Get into Blocked State
		xTaskNotifyWait(0, 0, NULL, portMAX_DELAY);
		// Unblock on RTC Interrupt
		printMsg("\n\r RT4 \n\r");
		// Open the curtains
		// Go back to sleep
	}
}



void FireAndTheftAlarm(void)
{
	// Disable other functions
	// Ring the alarm
	// Turn on all the lights
	// Send notification to phone
	// Wait until everything is normal, or a command is received from the phone
	// Re-Enable other functions
}



/* ********************************************* User Tasks' Definition ******************************************** */
void blinkLED(void)
{
	for(int i = 0; i<6; i++)
	{
		GPIO_ToggleBits(GPIOA, GPIO_Pin_5);
		for(int j = 0; j<500000; j++)
		{
			// wait
		}
	}
}



/* ********************************************** Interrupt Handlers *********************************************** */

void EXTI0_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line0);

	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	vToggleAppliance(Appl00_Fan_Cooler, &HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI1_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line1);

	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	vToggleAppliance(Appl01_Light1, &HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI2_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line2);

	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	vToggleAppliance(Appl02_Light2, &HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI3_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line3);

	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	vToggleAppliance(Appl03_, &HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI4_IRQHandler(void)
{
	EXTI_ClearITPendingBit(EXTI_Line4);

	BaseType_t HigherPriorityTaskWoken = pdFALSE;
	vToggleAppliance(Appl04_, &HigherPriorityTaskWoken);

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI9_5_IRQHandler(void)
{
	BaseType_t HigherPriorityTaskWoken = pdFALSE;



	if( EXTI_GetITStatus(EXTI_Line5) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line5);
		vToggleAppliance(Appl05_, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line6) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line6);
		vToggleAppliance(Appl06_, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line7) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line7);
		vToggleAppliance(Appl07_, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line8) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line8);
		vToggleAppliance(Appl08_, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line9) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line9);
		vToggleAppliance(Appl09_, &HigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



void EXTI15_10_IRQHandler(void)
{
	BaseType_t HigherPriorityTaskWoken = pdFALSE;

	if( EXTI_GetITStatus(EXTI_Line10) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line10);				// PB10 is not present on our NUCLEO Board
	}
	if( EXTI_GetITStatus(EXTI_Line11) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line11);				// PB11 is not present on our NUCLEO Board
	}
	if( EXTI_GetITStatus(EXTI_Line12) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line12);
		vToggleAppliance(Appl10_Curtains, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line13) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line13);
		vToggleAppliance(Appl11_NightLights, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line14) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line14);
		vToggleAppliance(Appl12_StaircaseLights, &HigherPriorityTaskWoken);
	}
	if( EXTI_GetITStatus(EXTI_Line15) == SET )
	{
		EXTI_ClearITPendingBit(EXTI_Line15);
		//vToggleAppliance(Appl13_Doorbell, &HigherPriorityTaskWoken);
	}

	portYIELD_FROM_ISR(HigherPriorityTaskWoken);
}



/* ****************************************** Interrupt Helper Functions ******************************************* */

void vToggleAppliance( uint8_t ApplianceID, BaseType_t* pxHigherPriorityTaskWoken)
{
	xApplianceCmdType CurrentCmd;

	uint8_t newApplianceState = ApplianceStates[ApplianceID] ^ 0x01;	// Toggle the appliance state

	CurrentCmd.ApplCmd = newApplianceState;
	CurrentCmd.ApplID = ApplianceID;

	sprintf(message, "\n\r Sending CMD(%d) for Appliance %d.", CurrentCmd.ApplCmd, CurrentCmd.ApplID);
	printMsg(message);

	// Send the command into the Appliance Command Queue for processing
	xQueueSendFromISR(xApplianceCmdQueue, &CurrentCmd, pxHigherPriorityTaskWoken);

	printMsg(" Command Sent.");

	// Notify the Appliance Control task that there's a command to be processed.
	xTaskNotifyFromISR(xOT2_H_ApplianceControl, 0, eNoAction, pxHigherPriorityTaskWoken);
}
