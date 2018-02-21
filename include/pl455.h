/*
 *  @file pl455.h
 *
 *  @author Stephen Holland - Texas Instruments Inc.
 *  @date June 2015
 *  @version 1.0 Initial version
 *  @note Built with CCS for Hercules Version: 5.5.0
 */

/*****************************************************************************
**
**  Copyright (c) 2011-2015 Texas Instruments
**
******************************************************************************/


#ifndef PL455_H_
#define PL455_H_

#include "hal_stdtypes.h"
#include "string.h"

#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "reg_rti.h"

// Define pl455 globals defines
#define TOTALBOARDS	16
#define BAUDRATE 250000

// Define pl455 global functions
void delayms(uint16 ms);
void delayus(uint16 us);

//typedef void (*pVoidFuncVoid)(void);
//typedef boolean (*pBoolFuncVoid)(void);

typedef struct pl455_main_s{
// Define public call functions
	uint8 numBoards;
	void (*Reset)(void);
	void (*Wakeup)(void);
	void (*ClearComms)(void);
	void (*ResetComms)(void);
	boolean (*GetFaultStat)(void);
	void (*ForceWakeup)(void);
	void (*PowerAllDown)(void);
	void (*MaskCheckSumFault)(void);
	void (*ClearAllFaults)(void);
	uint8 (*AutoAddress)(void);
	void (*EnableAllComs)(void);
//	void *ConfigureStackComms(void);
//	void *SetMuxDelay(uint16 delay);
//	void *SetInternalSamplePeriod(uint16 period);
//	void *SetOversampleRate(uint16 rate);
//	boolean *ClearCheckFaults(void);
} pl455_main_t;



pl455_main_t pl455_ctor(void);



#endif /* PL455_H_ */
//EOF
