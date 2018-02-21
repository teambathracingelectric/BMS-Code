/*
 *  @file pl455.c
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

#include "pl455.h"

/*****************************************************************************/
/* Structure functions definitions */

void ResetPL455();
void WakePL455();
void CommClear(void);
void CommReset(void);
boolean GetFaultStat();
void ForceWakeup(void);
void PowerAllDown(void);
void MaskCheckSumFault(void);
void ClearAllFaults(void);
uint8 AutoAddress(void);
void EnableAllComs(void);

/*****************************************************************************/
/* Private function definitions */

uint16 CRC16(uint8 *pBuf, int nLen);

uint16 B2SWORD(uint16 wIN);
uint32 B2SDWORD(uint32 dwIN);
uint32 B2SINT24(uint32 dwIN24);

uint32  WriteReg(uint8 bID, uint16 wAddr, uint64 dwData, uint8 bLen, uint8 bWriteType);
uint32  ReadReg(uint8 bID, uint16 wAddr, void * pData, uint8 bLen, uint32 dwTimeOut);

uint32  WriteFrame(uint8 bID, uint16 wAddr, uint8 * pData, uint8 bLen, uint8 bWriteType);
uint32  ReadFrameReq(uint8 bID, uint16 wAddr, uint8 buint8ToReturn);
uint32  WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut);

/*****************************************************************************/
/* Private Definitions */

// Write Type definitions
#define FRMWRT_SGL_R	0x00 // single device write with response
#define FRMWRT_SGL_NR	0x10 // single device write without response
#define FRMWRT_GRP_R	0x20 // group broadcast with response
#define FRMWRT_GRP_NR	0x30 // group broadcast without response
#define FRMWRT_ALL_R	0x60 // general broadcast with response
#define FRMWRT_ALL_NR	0x70 // general broadcast without response

// Register Addresses
#define SREV_REG		0x00 // Silicon Revision - R
#define	SREV_LEN		2
#define	CMD_REG			0x02 // Command - W
#define CMD_LEN			1
#define CHANNELS_REG	0x03 // Command channel select - R/W
#define	CHANNELS_LEN	4
#define	OVERSMPL_REG	0x07 // Command averaging (oversampling) - R/W
#define OVERSMPL_LEN	1
#define ADDR_REG		0x0A // Device address - R/W
#define	ADDR_LEN		1
#define	GROUP_ID_REG	0x0B // (Device) Group Identifier - R/W
#define GROUP_ID_LEN	1
#define DEV_CTRL_REG	0x0C // Device control - R/W
#define	DEV_CTRL_LEN	1
#define	NCHAN_REG		0x0D // Number of channels enabled for conversion - R/W
#define NCHAN_LEN		1
#define DEVCONFIG_REG	0x0E // Device configuration - R/W
#define	DEVCONFIG_LEN	1
#define	PWRCONFIG_REG	0x0F // Power configuration - R/W
#define PWRCONFIG_LEN	1
#define COMCONFIG_REG	0x10 // Communications configuration - R/W
#define	COMCONFIG_LEN	2
#define	TXHOLDOFF_REG	0x12 // UART Transmitter holdoff - R/W
#define TXHOLDOFF_LEN	1
#define CBCONFIG_REG	0x13 // Cell balancing (equalization) configuration - R/W
#define	CBCONFIG_LEN	1
#define	CBENBL_REG		0x14 // Cell balancing enables - R/W
#define CBENBL_LEN		2
#define TSTCONFIG_REG	0x1E // Built-In Self-Test (BIST) configuration - R/W
#define	TSTCONFIG_LEN	2
#define	TESTCTRL_REG	0x20 // BIST control - R/W
#define TESTCTRL_LEN	2
#define TEST_ADC_REG	0x22 // ADC BIST control - R/W
#define	TEST_ADC_LEN	3
#define	TESTAUXPU_REG	0x25 // Test control—AUX pull-up resistors - R/W
#define TESTAUXPU_LEN	1
#define CTO_REG			0x28 // Communications time-out - R/W
#define	CTO_LEN			1
#define	CTO_CNT_REG		0x29 // Communications time-out counter - R/W
#define CTO_CNT_LEN		3
#define AM_PER_REG		0x32 // Auto-monitor period - R/W
#define	AM_PER_LEN		1
#define	AM_CHAN_REG		0x33 // Auto-monitor channel select - R/W
#define AM_CHAN_LEN		4
#define AM_OSMPL_REG	0x37 // Auto-monitor averaging - R/W
#define	AM_OSMPL_LEN	1
#define	SMPL_DLY1_REG	0x3D // Initial sampling delay - R/W
#define SMPL_DLY1_LEN	1
#define CELL_SPER_REG	0x3E // Cell and die temperature measurement period - R/W
#define	CELL_SPER_LEN	1
#define	AUX_SPER_REG	0x3F // AUX channels sampling period - R/W
#define AUX_SPER_LEN	4
#define TEST_SPER_REG	0x43 // ADC test sampling period - R/W
#define	TEST_SPER_LEN	2
#define	SHDN_STS_REG	0x50 // Shutdown recovery status - R
#define SHDN_STS_LEN	1
#define STATUS_REG		0x51 // Device status - R/W
#define	STATUS_LEN		1
#define	FAULT_SUM_REG	0x52 // Fault summary - R/W
#define FAULT_SUM_LEN	4
#define FAULT_UV_REG	0x54 // Undervoltage faults - R/W
#define	FAULT_UV_LEN	4
#define	FAULT_OV_REG	0x56 // Overvoltage faults - R/W
#define FAULT_OV_LEN	4
#define FAULT_AUX_REG	0x58 // AUX threshold exceeded faults - R/W
#define	FAULT_AUX_LEN	4
#define	FAULT_2UV_REG	0x5A // Comparator UV faults - R/W
#define FAULT_2UV_LEN	4
#define FAULT_2OV_REG	0x5C // Comparator OV faults - R/W
#define	FAULT_2OV_LEN	4
#define	FAULT_COM_REG	0x5E // Communication faults - R/W
#define FAULT_COM_LEN	4
#define FAULT_SYS_REG	0x60 // System fault - R/W
#define	FAULT_SYS_LEN	1
#define	FAULT_DEV_REG	0x61 // Device fault - R/W
#define FAULT_DEV_LEN	2
#define FAULT_GPI_REG	0x63 // General purpose input (GPIO) fault - R/W
#define	FAULT_GPI_LEN	1
#define	MASK_COMM_REG	0x68 // Communications FAULT mask register - R/W
#define MASK_COMM_LEN	2
#define MASK_SYS_REG	0x6A // System FAULT mask register - R/W
#define	MASK_SYS_LEN	1
#define	MASK_DEV_REG	0x6B // Chip FAULT mask register - R/W
#define MASK_DEV_LEN	2
#define FO_CTRL_REG		0x6E // FAULT output control - R/W
#define	FO_CTRL_LEN		2
#define	GPIO_DIR_REG	0x78 // GPIO direction control - R/W
#define GPIO_DIR_LEN	1
#define GPIO_OUT_REG	0x79 // GPIO output control - R/W
#define	GPIO_OUT_LEN	1
#define	GPIO_PU_REG		0x7A // GPIO pull-up resistor control - R/W
#define GPIO_PU_LEN		1
#define GPIO_PD_REG		0x7B // GPIO pull-down resistor control - R/W
#define	GPIO_PD_LEN		1
#define	GPIO_IN_REG		0x7C // GPIO input value - R
#define GPIO_IN_LEN		1
#define GP_FLT_IN_REG	0x7D // GPIO input 0/1 FAULT assertion state - R/W
#define	GP_FLT_IN_LEN	1
#define	MAGIC1_REG		0x82 // "Magic" value enables EEPROM write - W
#define MAGIC1_LEN		4
#define COMP_UV_REG		0x8C // Comparator undervoltage threshold - R/W
#define	COMP_UV_LEN		1
#define	COMP_OV_REG		0x8D // Comparator overvoltage threshold - R/W
#define COMP_OV_LEN		1
#define CELL_UV_REG		0x8E // Cell undervoltage threshold - R/W
#define	CELL_UV_LEN		2
#define	CELL_OV_REG		0x90 // Cell overvoltage threshold - R/W
#define CELL_OV_LEN		2
#define AUX0_UV_REG		0x92 // AUX0 undervoltage threshold - R/W
#define	AUX0_UV_LEN		2
#define	AUX0_OV_REG		0x94 // AUX0 overvoltage threshold - R/W
#define AUX0_OV_LEN		2
#define AUX1_UV_REG		0x96 // AUX1 undervoltage threshold - R/W
#define	AUX1_UV_LEN		2
#define	AUX1_OV_REG		0x98 // AUX1 overvoltage threshold - R/W
#define AUX1_OV_LEN		2
#define AUX2_UV_REG		0x9A // AUX2 undervoltage threshold - R/W
#define	AUX2_UV_LEN		2
#define	AUX2_OV_REG		0x9C // AUX2 overvoltage threshold - R/W
#define AUX2_OV_LEN		2
#define AUX3_UV_REG		0x9E // AUX3 undervoltage threshold - R/W
#define	AUX3_UV_LEN		2
#define	AUX3_OV_REG		0xA0 // AUX3 overvoltage threshold - R/W
#define AUX3_OV_LEN		2
#define AUX4_UV_REG		0xA2 // AUX4 undervoltage threshold - R/W
#define	AUX4_UV_LEN		2
#define	AUX4_OV_REG		0xA4 // AUX4 overvoltage threshold - R/W
#define AUX4_OV_LEN		2
#define AUX5_UV_REG		0xA6 // AUX5 undervoltage threshold - R/W
#define	AUX5_UV_LEN		2
#define	AUX5_OV_REG		0xA8 // AUX5 overvoltage threshold - R/W
#define AUX5_OV_LEN		2
#define AUX6_UV_REG		0xAA // AUX6 undervoltage threshold - R/W
#define	AUX6_UV_LEN		2
#define	AUX6_OV_REG		0xAC // AUX6 overvoltage threshold - R/W
#define AUX6_OV_LEN		2
#define AUX7_UV_REG		0xAE // AUX7 undervoltage threshold - R/W
#define	AUX7_UV_LEN		2
#define	AUX7_OV_REG		0xB0 // AUX7 overvoltage threshold - R/W
#define AUX7_OV_LEN		2
#define LOT_NUM_REG		0xBE // Device Lot Number -  R
#define	LOT_NUM_LEN		8
#define	SER_NUM_REG		0xC6 // Device Serial Number - R
#define SER_NUM_LEN		2
#define SCRATCH_REG		0xC8 // User-defined data - R/W
#define	SCRATCH_LEN		8
#define	VSOFFSET_REG	0xD2 // ADC voltage offset correction - R/W
#define VSOFFSET_LEN	1
#define VSGAIN_REG		0xD3 // ADC voltage gain correction - R/W
#define	VSGAIN_LEN		1
#define	AX0OFFSET_REG	0xD4 // AUX0 ADC offset correction - R/W
#define AX0OFFSET_LEN	2
#define AX1OFFSET_REG	0xD6 // AUX1 ADC offset correction - R/W
#define	AX1OFFSET_LEN	2
#define	AX2OFFSET_REG	0xD8 // AUX2 ADC offset correction - R/W
#define AX2OFFSET_LEN	2
#define AX3OFFSET_REG	0xDA // AUX3 ADC offset correction - R/W
#define	AX3OFFSET_LEN	2
#define	AX4OFFSET_REG	0xDC // AUX4 ADC offset correction - R/W
#define AX4OFFSET_LEN	2
#define	AX5OFFSET_REG	0xDE // AUX5 ADC offset correction - R/W
#define AX5OFFSET_LEN	2
#define AX6OFFSET_REG	0xE0 // AUX6 ADC offset correction - R/W
#define	AX6OFFSET_LEN	2
#define	AX7OFFSET_REG	0xE2 // AUX7 ADC offset correction - R/W
#define AX7OFFSET_LEN	2
#define TSTR_ECC_REG	0xE6 // ECC Test Results - R
#define	TSTR_ECC_LEN	8
#define	CSUM_REG		0xF0 // Saved checksum value - R/W
#define CSUM_LEN		4
#define CSUM_RSLT_REG	0xF4 // Checksum Readout - R
#define	CSUM_RSLT_LEN	4
#define	TEST_CSUM_REG	0xF8 // Checksum Test Result - R
#define TEST_CSUM_LEN	2
#define EE_BURN_REG		0xFA // EEPROM Burn Count; up-counter - R
#define	EE_BURN_LEN		1
#define	MAGIC2_REG		0xFC // "Magic" value enables EEPROM write - W
#define MAGIC2_LEN		4

/*****************************************************************************/

extern int UART_RX_RDY;
extern int RTI_TIMEOUT;

/*****************************************************************************/
/* Structure Function Descriptions */

void ResetPL455()
{
//	ResetUart(1);
}

void WakePL455()
{
	// toggle wake signal
	gioSetBit(gioPORTA, 0, 0); // assert wake (active low)
	delayus(1000);
	gioToggleBit(gioPORTA, 0); // deassert wake
	delayus(1000);
}

void CommClear(void)
{
	int baudRate;
	baudRate = sciREG->BRS;

	sciREG->GCR1 &= ~(1U << 7U); // put SCI into reset
	sciREG->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	sciREG->PIO3 &= ~(1U << 2U); // set output to low

	delayus(baudRate * 2); // ~= 1/BAUDRATE/16*(155+1)*1.01
	sciInit();
	sciSetBaudrate(sciREG, BAUDRATE);
}

void CommReset(void)
{
	sciREG->GCR1 &= ~(1U << 7U); // put SCI into reset
	sciREG->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	sciREG->PIO3 &= ~(1U << 2U); // set output to low

	delayus(200); // should cover any possible baud rate
	sciInit();
	sciSetBaudrate(sciREG, BAUDRATE);
}

boolean GetFaultStat()
{

	if (!gioGetBit(gioPORTA, 1))
		return 0;
	return 1;
}

void ForceWakeup(void)
{
// Wake all devices
	// The wake tone will awaken any device that is already in shutdown and the pwrdown will shutdown any device
	// that is already awake. The least number of times to sequence wake and pwrdown will be half the number of
	// boards to cover the worst case combination of boards already awake or shutdown.
	uint8 nDev_ID;

	for(nDev_ID = 0; nDev_ID < TOTALBOARDS>>1; nDev_ID++) {
		WriteReg(nDev_ID, DEV_CTRL_REG, 0x40, DEV_CTRL_LEN, FRMWRT_ALL_NR);	// send out broadcast pwrdown command
		delayms(5); //~5ms
		WakePL455();
		delayms(5); //~5ms
	};
}

void PowerAllDown(void)
{
	uint8 nDev_ID;
	for(nDev_ID = 0; nDev_ID < TOTALBOARDS>>1; nDev_ID++) {
		WriteReg(nDev_ID, DEV_CTRL_REG, 0x40, DEV_CTRL_LEN, FRMWRT_ALL_NR);	// send out broadcast pwrdown command
	}
}

void MaskCheckSumFault(void)
{
	// Mask Customer Checksum Fault bit
	WriteReg(0, MASK_DEV_REG, 0x8000, MASK_DEV_LEN, FRMWRT_ALL_NR); // clear all fault summary flags
}

void ClearAllFaults(void)
{
	// Clear all faults
	WriteReg(0, FAULT_SUM_REG, 0xFFC0, FAULT_SUM_LEN, FRMWRT_ALL_NR);		// clear all fault summary flags
	WriteReg(0, STATUS_REG, 0x38, STATUS_LEN, FRMWRT_ALL_NR); // clear fault flags in the system status register
}

uint8 AutoAddress(void)
{
	uint8 nDev_ID;
	// Auto-address all boards (section 1.2.2)
	WriteReg(0, DEVCONFIG_REG, 0x19, DEVCONFIG_LEN, FRMWRT_ALL_NR); // set auto-address mode on all boards
	WriteReg(0, DEV_CTRL_REG, 0x08, DEV_CTRL_LEN, FRMWRT_ALL_NR); // enter auto address mode on all boards, the next write to this ID will be its address

	// Set addresses for all boards in daisy-chain (section 1.2.3)
	for (nDev_ID = 0; nDev_ID < TOTALBOARDS; nDev_ID++)
	{
		WriteReg(nDev_ID, ADDR_REG, nDev_ID, ADDR_LEN, FRMWRT_ALL_NR); // send address to each board
	}

	return nDev_ID;
}
void EnableAllComs(void)
{
	// Enable all communication interfaces on all boards in the stack (section 1.2.1)
	WriteReg(0, COMCONFIG_REG, 0x10F8, COMCONFIG_LEN, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces on all boards in stack
}

/*****************************************************************************/
/* Private Function descriptions */

uint32 WriteReg(uint8 bID, uint16 wAddr, uint64 dwData, uint8 bLen, uint8 bWriteType)
{
	int bRes = 0;
	uint8 bBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	switch(bLen)
	{
	case 1:
		bBuf[0] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 1, bWriteType);
		break;
	case 2:
		bBuf[0] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[1] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 2, bWriteType);
		break;
	case 3:
		bBuf[0] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[1] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[2] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 3, bWriteType);
		break;
	case 4:
		bBuf[0] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[1] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[2] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[3] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 4, bWriteType);
		break;
	case 5:
		bBuf[0] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[1] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[2] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[3] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[4] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 5, bWriteType);
		break;
	case 6:
		bBuf[0] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[1] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[2] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[3] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[4] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[5] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 6, bWriteType);
		break;
	case 7:
		bBuf[0] = (dwData & 0x00FF000000000000) >> 48;
		bBuf[1] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[2] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[3] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[4] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[5] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[6] =  dwData & 0x00000000000000FF;;
		bRes = WriteFrame(bID, wAddr, bBuf, 7, bWriteType);
		break;
	case 8:
		bBuf[0] = (dwData & 0xFF00000000000000) >> 56;
		bBuf[1] = (dwData & 0x00FF000000000000) >> 48;
		bBuf[2] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[3] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[4] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[5] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[6] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[7] =  dwData & 0x00000000000000FF;
		bRes = WriteFrame(bID, wAddr, bBuf, 8, bWriteType);
		break;
	default:
		break;
	}
	return bRes;
}

uint32  WriteFrame(uint8 bID, uint16 wAddr, uint8 * pData, uint8 bLen, uint8 bWriteType)
{
	int	   bPktLen = 0;
	uint8   pFrame[32];
	uint8 * pBuf = pFrame;
	uint16   wCRC;

	if (bLen == 7 || bLen > 8)
		return 0;

	memset(pFrame, 0x7F, sizeof(pFrame));
	if (wAddr > 255)	{
		*pBuf++ = 0x88 | bWriteType | bLen;	// use 16-bit address
		if (bWriteType == FRMWRT_SGL_R || bWriteType == FRMWRT_SGL_NR || bWriteType == FRMWRT_GRP_R || bWriteType == FRMWRT_GRP_NR)//(bWriteType != FRMWRT_ALL_NR)// || (bWriteType != FRMWRT_ALL_R))
		{
			*pBuf++ = (bID & 0x00FF);
		}
		*pBuf++ = (wAddr & 0xFF00) >> 8;
		*pBuf++ =  wAddr & 0x00FF;
	}
	else {
		*pBuf++ = 0x80 | bWriteType | bLen;	// use 8-bit address
		if (bWriteType == FRMWRT_SGL_R || bWriteType == FRMWRT_SGL_NR || bWriteType == FRMWRT_GRP_R || bWriteType == FRMWRT_GRP_NR)
		{
			*pBuf++ = (bID & 0x00FF);
		}
		*pBuf++ = wAddr & 0x00FF;
	}

	while(bLen--)
		*pBuf++ = *pData++;

	bPktLen = pBuf - pFrame;

	wCRC = CRC16(pFrame, bPktLen);
	*pBuf++ = wCRC & 0x00FF;
	*pBuf++ = (wCRC & 0xFF00) >> 8;
	bPktLen += 2;

	sciSend(sciREG, bPktLen, pFrame);

	return bPktLen;
}

uint32  ReadReg(uint8 bID, uint16 wAddr, void * pData, uint8 bLen, uint32 dwTimeOut)
{
	int   bRes = 0;
	uint8  bRX[8];

	memset(bRX, 0, sizeof(bRX));
	switch(bLen)
	{
	case 1:
		bRes = ReadFrameReq(bID, wAddr, 1);
		bRes = WaitRespFrame(bRX, 4, dwTimeOut);
		if (bRes == 1)
			*((uint8 *)pData) = bRX[1] & 0x00FF;
		else
			bRes = 0;
		break;
	case 2:
		bRes = ReadFrameReq(bID, wAddr, 2);
		bRes = WaitRespFrame(bRX, 5, dwTimeOut);
		if (bRes == 2)
			*((uint16 *)pData) = ((uint16)bRX[1] << 8) | (bRX[2] & 0x00FF);
		else
			bRes = 0;
		break;
	case 3:
		bRes = ReadFrameReq(bID, wAddr, 3);
		bRes = WaitRespFrame(bRX, 6, dwTimeOut);
		if (bRes == 3)
			*((uint32 *)pData) = ((uint32)bRX[1] << 16) | ((uint16)bRX[2] << 8) | (bRX[3] & 0x00FF);
		else
			bRes = 0;
		break;
	case 4:
		bRes = ReadFrameReq(bID, wAddr, 4);
		bRes = WaitRespFrame(bRX, 7, dwTimeOut);
		if (bRes == 4)
			*((uint32 *)pData) = ((uint32)bRX[1] << 24) | ((uint32)bRX[2] << 16) | ((uint16)bRX[3] << 8) | (bRX[4] & 0x00FF);
		else
			bRes = 0;
		break;
	default:
		break;
	}
	return bRes;
}

uint32 ReadFrameReq(uint8 bID, uint16 wAddr, uint8 buint8ToReturn)
{
	uint8 bReturn = buint8ToReturn - 1;

	if (bReturn > 127)
		return 0;

	return WriteFrame(bID, wAddr, &bReturn, 1, FRMWRT_SGL_R);
}

uint32  WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut)
{
	uint16 wCRC = 0, wCRC16;
	uint8 bBuf[132];
	uint8 bRxDataLen;

	memset(bBuf, 0, sizeof(bBuf));

	sciEnableNotification(sciREG, SCI_RX_INT);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
	 /* rtiNOTIFICATION_COMPARE0 = 1ms
	 *  rtiNOTIFICATION_COMPARE1 = 5ms
	 *  rtiNOTIFICATION_COMPARE2 = 8ms
	 *  rtiNOTIFICATION_COMPARE3 = 10ms
	 */
	sciReceive(sciREG, bLen, bBuf);
	rtiResetCounter(rtiCOUNTER_BLOCK0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);

	while(UART_RX_RDY == 0U)
	{
		// Check for timeout.
		if(RTI_TIMEOUT == 1U)
		{
			RTI_TIMEOUT = 0;
			return 0; // timed out
		}
	} /* Wait */
	rtiStopCounter(rtiCOUNTER_BLOCK0);

	UART_RX_RDY = 0;
	bRxDataLen = bBuf[0];

	delayms(dwTimeOut);

	// rebuild bBuf to have bLen as first uint8 to use the same CRC function as TX
//	i = bRxDataLen + 3;
//	while(--i >= 0)
//	{
//		bBuf[i + 1] = bBuf[i];
//	}
//	bBuf[0] = bRxDataLen;

	wCRC = bBuf[bRxDataLen+2];
	wCRC |= ((uint16)bBuf[bRxDataLen+3] << 8);
	wCRC16 = CRC16(bBuf, bRxDataLen+2);
	if (wCRC != wCRC16)
		return -1;

	memcpy(pFrame, bBuf, bRxDataLen + 4);

	return bRxDataLen + 1;
}

// Big endian / Little endian conversion
uint16  B2SWORD(uint16 wIN)
{
	uint16 wOUT = 0;

	wOUT =   wIN & 0x00FF >> 8;
	wOUT |= (wIN & 0xFF00) >> 8;

	return wOUT;
}

uint32 B2SDWORD(uint32 dwIN)
{
	uint32 dwOUT = 0;

	dwOUT =   dwIN & 0x00FF >> 8;
	dwOUT |= (dwIN & 0xFF00) >> 8;
	dwOUT |=  dwIN & 0x00FF >> 8;
	dwOUT |= (dwIN & 0xFF00) >> 8;

	return dwOUT;
}

uint32 B2SINT24(uint32 dwIN24)
{
	uint32 dwOUT = 0;

	dwOUT =   dwIN24 & 0x00FF >> 8;
	dwOUT |= (dwIN24 & 0xFF00) >> 8;
	dwOUT |=  dwIN24 & 0x00FF >> 8;

	return dwOUT;
}

// CRC16 for PL455
// ITU_T polynomial: x^16 + x^15 + x^2 + 1
const uint16 crc16_table[256] = {
	0x0000, 0xC0C1, 0xC181, 0x0140, 0xC301, 0x03C0, 0x0280, 0xC241,
	0xC601, 0x06C0, 0x0780, 0xC741, 0x0500, 0xC5C1, 0xC481, 0x0440,
	0xCC01, 0x0CC0, 0x0D80, 0xCD41, 0x0F00, 0xCFC1, 0xCE81, 0x0E40,
	0x0A00, 0xCAC1, 0xCB81, 0x0B40, 0xC901, 0x09C0, 0x0880, 0xC841,
	0xD801, 0x18C0, 0x1980, 0xD941, 0x1B00, 0xDBC1, 0xDA81, 0x1A40,
	0x1E00, 0xDEC1, 0xDF81, 0x1F40, 0xDD01, 0x1DC0, 0x1C80, 0xDC41,
	0x1400, 0xD4C1, 0xD581, 0x1540, 0xD701, 0x17C0, 0x1680, 0xD641,
	0xD201, 0x12C0, 0x1380, 0xD341, 0x1100, 0xD1C1, 0xD081, 0x1040,
	0xF001, 0x30C0, 0x3180, 0xF141, 0x3300, 0xF3C1, 0xF281, 0x3240,
	0x3600, 0xF6C1, 0xF781, 0x3740, 0xF501, 0x35C0, 0x3480, 0xF441,
	0x3C00, 0xFCC1, 0xFD81, 0x3D40, 0xFF01, 0x3FC0, 0x3E80, 0xFE41,
	0xFA01, 0x3AC0, 0x3B80, 0xFB41, 0x3900, 0xF9C1, 0xF881, 0x3840,
	0x2800, 0xE8C1, 0xE981, 0x2940, 0xEB01, 0x2BC0, 0x2A80, 0xEA41,
	0xEE01, 0x2EC0, 0x2F80, 0xEF41, 0x2D00, 0xEDC1, 0xEC81, 0x2C40,
	0xE401, 0x24C0, 0x2580, 0xE541, 0x2700, 0xE7C1, 0xE681, 0x2640,
	0x2200, 0xE2C1, 0xE381, 0x2340, 0xE101, 0x21C0, 0x2080, 0xE041,
	0xA001, 0x60C0, 0x6180, 0xA141, 0x6300, 0xA3C1, 0xA281, 0x6240,
	0x6600, 0xA6C1, 0xA781, 0x6740, 0xA501, 0x65C0, 0x6480, 0xA441,
	0x6C00, 0xACC1, 0xAD81, 0x6D40, 0xAF01, 0x6FC0, 0x6E80, 0xAE41,
	0xAA01, 0x6AC0, 0x6B80, 0xAB41, 0x6900, 0xA9C1, 0xA881, 0x6840,
	0x7800, 0xB8C1, 0xB981, 0x7940, 0xBB01, 0x7BC0, 0x7A80, 0xBA41,
	0xBE01, 0x7EC0, 0x7F80, 0xBF41, 0x7D00, 0xBDC1, 0xBC81, 0x7C40,
	0xB401, 0x74C0, 0x7580, 0xB541, 0x7700, 0xB7C1, 0xB681, 0x7640,
	0x7200, 0xB2C1, 0xB381, 0x7340, 0xB101, 0x71C0, 0x7080, 0xB041,
	0x5000, 0x90C1, 0x9181, 0x5140, 0x9301, 0x53C0, 0x5280, 0x9241,
	0x9601, 0x56C0, 0x5780, 0x9741, 0x5500, 0x95C1, 0x9481, 0x5440,
	0x9C01, 0x5CC0, 0x5D80, 0x9D41, 0x5F00, 0x9FC1, 0x9E81, 0x5E40,
	0x5A00, 0x9AC1, 0x9B81, 0x5B40, 0x9901, 0x59C0, 0x5880, 0x9841,
	0x8801, 0x48C0, 0x4980, 0x8941, 0x4B00, 0x8BC1, 0x8A81, 0x4A40,
	0x4E00, 0x8EC1, 0x8F81, 0x4F40, 0x8D01, 0x4DC0, 0x4C80, 0x8C41,
	0x4400, 0x84C1, 0x8581, 0x4540, 0x8701, 0x47C0, 0x4680, 0x8641,
	0x8201, 0x42C0, 0x4380, 0x8341, 0x4100, 0x81C1, 0x8081, 0x4040
};

uint16 CRC16(uint8 *pBuf, int nLen)
{
	uint16 wCRC = 0;
	int i;

	for (i = 0; i < nLen; i++)
	{
		wCRC ^= (*pBuf++) & 0x00FF;
		wCRC = crc16_table[wCRC & 0x00FF] ^ (wCRC >> 8);
	}

	return wCRC;
}

void delayms(uint16 ms) {
	  volatile unsigned int delayval;
	  delayval = ms * 8400;   // 8400 are about 1ms
	  while(delayval--);
}

void delayus(uint16 us) {
	  static volatile unsigned int delayval;
	  delayval = us * 9;
	  while(delayval--);
}



pl455_main_t pl455_ctor(void) {
	// Initialise Structure
	pl455_main_t main;

	// Setup function pointers
	main.ClearComms = CommClear;
	main.GetFaultStat = GetFaultStat;
	main.Reset = ResetPL455;
	main.ResetComms = CommReset;
	main.Wakeup = WakePL455;
	main.ForceWakeup = ForceWakeup;
	main.AutoAddress = AutoAddress;
	main.ClearAllFaults = ClearAllFaults;
	main.EnableAllComs = EnableAllComs;
	main.MaskCheckSumFault = MaskCheckSumFault;
	main.PowerAllDown = PowerAllDown;

	// Initialise slave and setup parameters etc
	main.ClearComms();
	main.ResetComms();
	main.ForceWakeup();
	main.MaskCheckSumFault();
	main.ClearAllFaults();
	main.numBoards = main.AutoAddress();

	return main;
}


//EOF

