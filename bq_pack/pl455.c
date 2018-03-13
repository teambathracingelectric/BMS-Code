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


#include "string.h"

#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "reg_rti.h"

#include "pl455.h"

extern boolean UART_RX_RDY;
extern boolean RTI_TIMEOUT;
/******************************************************************************/
/*                       Global functions declaration                          */
/******************************************************************************/
sint32 bq_InitialiseStack(void)
{
	/** Code examples
	 * The command sequences below are examples of the message examples in the bq76PL455 Communication Examples document.
	 * each message example references the section in the document.
	 */

	sint16 nDev_ID = 0;
	sint32 nRead;
	sint16 nTopFound;
	uint8 wTemp;


/*************************** Wake all devices *********************************/
	// The wake tone will awaken any device that is already in shutdown and the pwrdown will shutdown any device
	// that is already awake. The least number of times to sequence wake and pwrdown will be half the number of
	// boards to cover the worst case combination of boards already awake or shutdown.
	for(nDev_ID = 0; nDev_ID < BQ_NUMBER_OF_DEVICES>>1; nDev_ID++) {
		bq_WriteReg(nDev_ID, DEV_CTRL_REG, 0x40, 1, FRMWRT_ALL_NR);	// send out broadcast pwrdown command
		delayms(5); //~5ms
		bq_Wakeup();
		delayms(5); //~5ms
	}

/************************ Auto-Address all boards ******************************/
	// Mask Customer Checksum Fault bit
	bq_WriteReg(0, MASK_DEV_REG, 0x8000, 2, FRMWRT_ALL_NR); // clear all fault summary flags

	// Clear all faults
	bq_WriteReg(0, FAULT_SUM_REG, 0xFFC0, 2, FRMWRT_ALL_NR);		// clear all fault summary flags
	bq_WriteReg(0, STATUS_REG, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

	// Auto-address all boards (section 1.2.2)
	bq_WriteReg(0, DEVCONFIG_REG, 0x19, 1, FRMWRT_ALL_NR); // set auto-address mode on all boards
	bq_WriteReg(0, DEV_CTRL_REG, 0x08, 1, FRMWRT_ALL_NR); // enter auto address mode on all boards, the next write to this ID will be its address

	// Set addresses for all boards in daisy-chain (section 1.2.3)
	for (nDev_ID = 0; nDev_ID < BQ_NUMBER_OF_DEVICES; nDev_ID++)
	{
		bq_WriteReg(nDev_ID, ADDR_REG, nDev_ID, 1, FRMWRT_ALL_NR); // send address to each board
	}

/************************ Setup all devices comms ******************************/
	// Enable all communication interfaces on all boards in the stack (section 1.2.1)
	bq_WriteReg(0, COMCONFIG_REG, 0x10F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces on all boards in stack

	/* Change to final baud rate used in the application (set by BAUDRATE define in pl455.h).
	 * Up to this point, all communication is at 250Kb, as the COMM_RESET done at the initial
	 * startup resets the bq76PL455A-Q1 UART to 250Kb. */
	switch(BQ_BAUDRATE)
	{
	case 125000:
		bq_WriteReg(0, COMCONFIG_REG, 0x00F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
		delayms(1);
		sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
		break;
	case 250000:
		delayms(1);
		break;
	case 500000:
		bq_WriteReg(0, COMCONFIG_REG, 0x20F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
		delayms(1);
		sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
		break;
	case 1000000:
		bq_WriteReg(0, COMCONFIG_REG, 0x30F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
		delayms(1);
		sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
		break;
	}

	/* Set communications interfaces appropriately for their position in the stack, and
	 * for baud rate used in the application (set by BAUDRATE define in pl455.h).
	 * (section 1.2.4)
	 */
	for (nDev_ID = BQ_NUMBER_OF_DEVICES - 1; nDev_ID >= 0; --nDev_ID)
	{
		// read device ID to see if there is a response
		nRead = bq_ReadReg(nDev_ID, ADDR_REG, &wTemp, 1, 1); // 1ms timeout

//		if(nRead < 0)
//			return -1;
		if(nRead <= 0) // if nothing is read then this board doesn't exist
			nTopFound = 0;
		else // a response was received
		{
			if(nTopFound == 0)
			{ // if the last board was not present but this one is, this is the top board
				if(nDev_ID == 0) // this is the only board
				{
					switch(BQ_BAUDRATE)
						{
						case 125000:
							bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x0080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 250000:
							bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x1080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 500000:
							bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x2080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 1000000:
							bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x3080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						}
				}
				else // this is the top board of a stack (section 1.2.5)
				{
					switch(BQ_BAUDRATE)
					{
					case 125000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x0028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 250000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x1028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 500000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x2028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 1000000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x3028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					}
					nTopFound = 1;
				}
			}
			else // this is a middle or bottom board
			{
				if(nDev_ID == 0) // this is a bottom board of a stack (section 1.2.6)
				{
					switch(BQ_BAUDRATE)
					{
					case 125000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x00D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 250000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x10D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 500000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x20D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 1000000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x30D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					}
				}
				else // this is a middle board
				{
					switch(BQ_BAUDRATE)
					{
					case 125000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x0078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 250000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x1078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 500000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x2078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 1000000:
						bq_WriteReg(nDev_ID, COMCONFIG_REG, 0x3078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					}
				}
			}
		}
	}

	// Clear all faults (section 1.2.7)
	bq_WriteReg(0, FAULT_SUM_REG, 0xFFC0, 2, FRMWRT_ALL_NR); // clear all fault summary flags
	bq_WriteReg(0, STATUS_REG, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

	delayms(10);


/*************************** Setup AFE *********************************/
	// Configure AFE (section 2.2.1)

	bq_WriteReg(nDev_ID, SMPL_DLY1_REG, 0x00, 1, FRMWRT_ALL_NR); // set 0 initial delay

	// Configure voltage and internal sample period (section 2.2.2)
	bq_WriteReg(nDev_ID, CELL_SPER_REG, 0xCC, 1, FRMWRT_ALL_NR); // set 99.92us ADC sampling period

	// Configure the oversampling rate (section 2.2.3)
	bq_WriteReg(nDev_ID, OVERSMPL_REG, 0x00, 1, FRMWRT_ALL_NR); // set no oversampling period

//	// Clear and check faults (section 2.2.4) --->> Only on first board at the moment, but don't need to use. need to make seperate function
//	nDev_ID = 0;
//	bq_WriteReg(nDev_ID, STATUS_REG, 0x38, 1, FRMWRT_SGL_NR); // clear fault flags in the system status register
//	bq_WriteReg(nDev_ID, FAULT_SUM_REG, 0xFFC0, 2, FRMWRT_SGL_NR); // clear all fault summary flags
//	nRead = bq_ReadReg(nDev_ID, STATUS_REG, &wTemp, 1, 0); // 0ms timeout
//	nRead = bq_ReadReg(nDev_ID, FAULT_SUM_REG, &wTemp, 2, 0); // 0ms timeout

//	// Select number of cells and channels to sample (section 2.2.5.1)
//	nDev_ID = 0;
//	bq_WriteReg(nDev_ID, NCHAN_REG, 0x10, 1, FRMWRT_SGL_NR); // set number of cells to 16
//	bq_WriteReg(nDev_ID, CHANNELS_REG, 0xFFFF03C0, 4, FRMWRT_SGL_NR); // select all cell, AUX channels 0 and 1, and internal digital die and internal analog die temperatures

	// Select identical number of cells and channels on all modules simultaneously (section 2.2.5.2)
	bq_WriteReg(0, NCHAN_REG, 0x10, 1, FRMWRT_ALL_NR); // set number of cells to 16
	bq_WriteReg(0, CHANNELS_REG, 0xFFFF0000, 4, FRMWRT_ALL_NR); // select all cell, no AUX channels, and no temperatures

//	// Set cell over-voltage and cell under-voltage thresholds on a single board (section 2.2.6.1)
//	nDev_ID = 0;
//	bq_WriteReg(nDev_ID, 144, 0xD1EC, 2, FRMWRT_SGL_NR); // set OV threshold = 4.1000V
//	bq_WriteReg(nDev_ID, 142, 0x6148, 2, FRMWRT_SGL_NR); // set UV threshold = 1.9000V

	// Set cell over-voltage and cell under-voltage thresholds on all boards simultaneously (section 2.2.6.2)
	bq_WriteReg(0, CELL_OV_REG, 0xD1EC, 2, FRMWRT_ALL_NR); // set OV threshold = 4.1000V
	bq_WriteReg(0, CELL_UV_REG, 0x6148, 2, FRMWRT_ALL_NR); // set UV threshold = 1.9000V

	return 0;
}


void bq_Wakeup(void)
{
	// toggle wake signal
	gioSetBit(gioPORTA, 0, 0); // assert wake (active low)
	delayus(10);
	gioToggleBit(gioPORTA, 0); // deassert wake
}

void bq_Sample_SGL(uint8 bID, bq_dev_sample_data_t * data)
{
	uint8  nPackets = ( 16 + 2 + 1 + 2 ); // 16 vcells, 0 temperatures, 2 internals temps, 1 header, 2 CRC
	uint8  bFrame[( 16 + 2 + 1 + 2 )];
	uint8 i = 0;
	sint32 responseReturn;

	// Send broadcast request to all boards to sample and send results (section 3.2)
	bq_WriteReg(bID, CMD_REG, 0x02, 1, FRMWRT_SGL_NR); // send sync sample command
	responseReturn = bq_WaitRespFrame(bFrame, nPackets, 5); // 24 bytes data (x3) + packet header (x3) + CRC (x3), 0ms timeout

	if( responseReturn > 0 ){
		for(i = 1; i<nPackets-4; i++){
			data->cell_voltage[i-1] = bFrame[i];
		}
		data->internal_digital_temp = bFrame[17];
		data->internal_analogue_temp = bFrame[18];
	}
	else return;

}

void bq_Sample_ALL(bq_dev_sample_data_t * data)
{
	uint8  nPackets = (2*16) + 1 + 2; // 16 vcells, 0 temperatures, 1 header, 2 CRC
	uint8  bFrame[(2*16) + 1 + 2];
	uint8  * pbFrame = bFrame;
	uint8 i = 0;
	sint32 responseReturn;

	// Send broadcast request to all boards to sample and send results (section 3.2)
	bq_WriteReg(0, CMD_REG, 0x00, 1, FRMWRT_ALL_R); // send sync sample command
	responseReturn = bq_WaitRespFrame(bFrame, nPackets, 5); // 24 bytes data (x3) + packet header (x3) + CRC (x3), 0ms timeout

	if( responseReturn > 0 ){
		for(i = 0; i<16; i++){
			data->cell_voltage[i] = bq_sample_to_voltage(pbFrame + 1 + (2*i));
		}
//		data->internal_digital_temp = bq_sample_to_voltage(pbFrame +nPackets-2-3);
//		data->internal_analogue_temp = bq_sample_to_voltage(pbFrame +nPackets-2-1);
	}
	else return;

}

uint16 bq_sample_to_voltage(uint8 * pFrames)
{
	uint16 sample = 0;
	float32 calc = 0;

	sample = ( (*pFrames+1 << 8) & 0xFF00 ) | ( *pFrames & 0x00FF );

	calc = sample*5000/0xFFFF;

	return (uint16)(calc);
}

//void bq_Shutdown(void)
//{
//
//}
//
//void bq_ResetStack(void)
//{
//	// To fill
//}
//
//void bq_SaveConfig(void)
//{
//	// To fill
//}
//
//uint16 bq_GetFaults(void)
//{
//	// To fill
//	return 0;
//}

/******************************************************************************/
/*                       Local functions declaration                          */
/******************************************************************************/


uint16 CRC16(uint8 *pBuf, uint16 nLen);

void CommClear(void)
{
	int baudRate;
	baudRate = BQ_UART->BRS;

	BQ_UART->GCR1 &= ~(1U << 7U); // put SCI into reset
	BQ_UART->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	BQ_UART->PIO3 &= ~(1U << 2U); // set output to low

	delayus(baudRate * 2); // ~= 1/BAUDRATE/16*(155+1)*1.01
	sciInit();
	sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
}

void CommReset(void)
{
	BQ_UART->GCR1 &= ~(1U << 7U); // put SCI into reset
	BQ_UART->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	BQ_UART->PIO3 &= ~(1U << 2U); // set output to low

	delayus(200); // should cover any possible baud rate
	sciInit();
	sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
}

void ResetPL455(void)
{
//	ResetUart(1);
}

boolean GetFaultStat(void)
{

	if (!gioGetBit(gioPORTA, 1))
		return 0;
	return 1;
}

sint32  bq_WriteReg(uint8 bID, bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen, bq_write_type_t bWriteType)
{
	sint32 bRes = 0;
	uint8 bBuf[8] = {0, 0, 0, 0, 0, 0, 0, 0};
	switch(bLen)
	{
	case 1:
		bBuf[0] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 1, bWriteType);
		break;
	case 2:
		bBuf[0] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[1] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 2, bWriteType);
		break;
	case 3:
		bBuf[0] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[1] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[2] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 3, bWriteType);
		break;
	case 4:
		bBuf[0] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[1] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[2] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[3] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 4, bWriteType);
		break;
	case 5:
		bBuf[0] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[1] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[2] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[3] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[4] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 5, bWriteType);
		break;
	case 6:
		bBuf[0] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[1] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[2] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[3] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[4] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[5] =  dwData & 0x00000000000000FF;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 6, bWriteType);
		break;
	case 7:
		bBuf[0] = (dwData & 0x00FF000000000000) >> 48;
		bBuf[1] = (dwData & 0x0000FF0000000000) >> 40;
		bBuf[2] = (dwData & 0x000000FF00000000) >> 32;
		bBuf[3] = (dwData & 0x00000000FF000000) >> 24;
		bBuf[4] = (dwData & 0x0000000000FF0000) >> 16;
		bBuf[5] = (dwData & 0x000000000000FF00) >> 8;
		bBuf[6] =  dwData & 0x00000000000000FF;;
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 7, bWriteType);
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
		bRes = bq_WriteFrame(bID, wAddr, bBuf, 8, bWriteType);
		break;
	default:
		break;
	}
	return bRes;
}

sint32  bq_WriteFrame(uint8 bID, bq_dev_regs_t wAddr, uint8 * pData, uint8 bLen, bq_write_type_t bWriteType)
{
	sint32	   bPktLen = 0;
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

	sciSend(BQ_UART, bPktLen, pFrame);

	return bPktLen;
}

sint32  bq_ReadReg(uint8 bID, bq_dev_regs_t wAddr, void * pData, uint8 bLen, uint32 dwTimeOut)
{
	sint32   bRes = 0;
	uint8  bRX[8];

	memset(bRX, 0, sizeof(bRX));
	switch(bLen)
	{
	case 1:
		bRes = bq_ReadFrameReq(bID, wAddr, 1);
		bRes = bq_WaitRespFrame(bRX, 4, dwTimeOut);
		if (bRes == 1)
			*((uint8 *)pData) = bRX[1] & 0x00FF;
		else
			bRes = -2;
		break;
	case 2:
		bRes = bq_ReadFrameReq(bID, wAddr, 2);
		bRes = bq_WaitRespFrame(bRX, 5, dwTimeOut);
		if (bRes == 2)
			*((uint16 *)pData) = ((uint16)bRX[1] << 8) | (bRX[2] & 0x00FF);
		else
			bRes = -2;
		break;
	case 3:
		bRes = bq_ReadFrameReq(bID, wAddr, 3);
		bRes = bq_WaitRespFrame(bRX, 6, dwTimeOut);
		if (bRes == 3)
			*((uint32 *)pData) = ((uint32)bRX[1] << 16) | ((uint16)bRX[2] << 8) | (bRX[3] & 0x00FF);
		else
			bRes = -2;
		break;
	case 4:
		bRes = bq_ReadFrameReq(bID, wAddr, 4);
		bRes = bq_WaitRespFrame(bRX, 7, dwTimeOut);
		if (bRes == 4)
			*((uint32 *)pData) = ((uint32)bRX[1] << 24) | ((uint32)bRX[2] << 16) | ((uint16)bRX[3] << 8) | (bRX[4] & 0x00FF);
		else
			bRes = -2;
		break;
	default:
		bRes = -3; // return bLen out of range
		break;
	}
	return bRes;
}

sint32  bq_ReadFrameReq(uint8 bID, bq_dev_regs_t wAddr, uint8 bByteToReturn)
{
	uint8 bReturn = bByteToReturn - 1;

	if (bReturn > 127)
		return 0;

	return bq_WriteFrame(bID, wAddr, &bReturn, 1, FRMWRT_SGL_R);
}

sint32  bq_WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut)
{
	uint16 wCRC = 0, wCRC16;
	uint8 bBuf[132];
	uint8 bRxDataLen;

	memset(bBuf, 0, sizeof(bBuf));

	sciEnableNotification(BQ_UART, SCI_RX_INT);
	rtiEnableNotification(rtiNOTIFICATION_COMPARE3);
	 /* rtiNOTIFICATION_COMPARE0 = 1ms
	 *  rtiNOTIFICATION_COMPARE1 = 5ms
	 *  rtiNOTIFICATION_COMPARE2 = 8ms
	 *  rtiNOTIFICATION_COMPARE3 = 10ms
	 */
	sciReceive(BQ_UART, bLen, bBuf);
	rtiResetCounter(rtiCOUNTER_BLOCK0);
	rtiStartCounter(rtiCOUNTER_BLOCK0);

	while(UART_RX_RDY == 0)
	{
		// Check for timeout.
		if(RTI_TIMEOUT == 1)
		{
			RTI_TIMEOUT = 0;
			return -2; // timed out error = -2
		}
	} /* Wait */
	rtiStopCounter(rtiCOUNTER_BLOCK0);

	UART_RX_RDY = 0;
	bRxDataLen = bBuf[0];

	delayms(dwTimeOut);

	// rebuild bBuf to have bLen as first BYTE to use the same CRC function as TX
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
		return -1; // CRC check error = -1

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

uint16 CRC16(uint8 *pBuf, uint16 nLen)
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
//EOF

