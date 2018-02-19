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

//#include "datatypes.h"

// User defines
#define FRMWRT_SGL_R	0x00 // single device write with response
#define FRMWRT_SGL_NR	0x10 // single device write without response
#define FRMWRT_GRP_R	0x20 // group broadcast with response
#define FRMWRT_GRP_NR	0x30 // group broadcast without response
#define FRMWRT_ALL_R	0x60 // general broadcast with response
#define FRMWRT_ALL_NR	0x70 // general broadcast without response

#define TOTALBOARDS	16
#define BAUDRATE 250000

// Function Prototypes
void ResetPL455();
void WakePL455();
void CommClear(void);
void CommReset(void);
boolean GetFaultStat();

uint16  B2SWORD(uint16 wIN);
uint32 B2SDWORD(uint32 dwIN);
uint32 B2SINT24(uint32 dwIN24);

sint32  WriteReg(uint8 bID, uint16 wAddr, uint64 dwData, uint8 bLen, uint8 bWriteType);
sint32  ReadReg(uint8 bID, uint16 wAddr, void * pData, uint8 bLen, uint32 dwTimeOut);

sint32  WriteFrame(uint8 bID, uint16 wAddr, uint8 * pData, uint8 bLen, uint8 bWriteType);
sint32  ReadFrameReq(uint8 bID, uint16 wAddr, uint8 buint8ToReturn);
sint32  WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut);

void delayms(uint16 ms);
void delayus(uint16 us);

#endif /* PL455_H_ */

/* Testing writing new class */
/*

class pl455_s
	{
public:
	#define TOTALBOARDS	16
	#define BAUDRATE 250000

	void Reset();
	void Wakeup();
	void CommClear(void);
	void CommReset(void);
	boolean GetFaultStat();
private:
	#define FRMWRT_SGL_R	0x00 // single device write with response
	#define FRMWRT_SGL_NR	0x10 // single device write without response
	#define FRMWRT_GRP_R	0x20 // group broadcast with response
	#define FRMWRT_GRP_NR	0x30 // group broadcast without response
	#define FRMWRT_ALL_R	0x60 // general broadcast with response
	#define FRMWRT_ALL_NR	0x70 // general broadcast without response

	};

 */

//EOF
