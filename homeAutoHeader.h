#ifndef HOME_AUTOMATION_HEADER_H
#define HOME_AUTOMATION_HEADER_H

#ifdef __cplusplus
	extern "C" {
#endif

#include "myHeaderFile.h"

#define ApplianceSwitchGPIOPort		GPIOB
// 14 Pins Available
#define TotalSwitchPins	14
extern uint16_t ApplSwitchPins[14];


#define ApplianceRelayGPIOPort		GPIOA
// 13 Pins Available
#define TotalRelayPins	13
extern uint16_t ApplRelayPins[13];

void Initialize_Home_Automation_Things(void);


#ifdef __cplusplus
	extern "C" {
#endif

#endif
