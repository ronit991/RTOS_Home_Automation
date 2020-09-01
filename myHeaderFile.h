#ifndef MYHEADERFILE_H
#define MYHEADERFILE_H

#ifdef __cplusplus
 extern "C" {
#endif

 // Standard Library files
 #include <stdio.h>
 #include <stdlib.h>
 #include <stdint.h>
 #include <string.h>

 // Architecture/System Specific files
 #include "stm32f4xx.h"

 // FreeRTOS Library files
 #include "FreeRTOS.h"
 #include "task.h"
 #include "timers.h"
 #include "queue.h"
 #include "semphr.h"


 // Flags
 #define False		0
 #define True		1

 #define Set		True
 #define Clear		False

 #define Yes		True
 #define No			False

 // Setup MCU and Board Specific Peripherals
 void prvSetupHW(void);
 void prvSetupNucleoButtonAndLED(void);
 void prvSetupUART(void);

 // Print a message over UART
 void printMsg(char *msg);


 // Starting code for every application
 void Initialize(void);


#ifdef __cplusplus
 }
#endif

#endif
