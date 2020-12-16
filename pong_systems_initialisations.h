
#ifndef PONG_SYSTEMS_INITIALISATIONS_H_
#define PONG_SYSTEMS_INITIALISATIONS_H_


#endif /* PONG_SYSTEMS_INITIALISATIONS_H_ */

#ifndef MPS430F5438A_H
#define MPS430F5438A_H
#include    "msp430f5438a.h"
#endif

#ifndef HAL_HEADERS_H
#define HAL_HEADERS_H
#include    "hal_UCS.h"
#include    "hal_PMM.h"
#include    "hal_lcd.h"
#endif

#ifndef GENERAL_SETTINGS_H
#define GENERAL_SETTINGS_H
#include    "general_settings.h"
#endif

void halBoardInit(void);
void LCDInit(void);
void TimerA1Init(void);
