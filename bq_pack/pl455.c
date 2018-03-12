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


/******************************************************************************/
/*                       Global functions declaration                          */
/******************************************************************************/
void bqInitialiseStack(void)
{
	/** Code examples
	 * The command sequences below are examples of the message examples in the bq76PL455 Communication Examples document.
	 * each message example references the section in the document.
	 */

	sin16 nDev_ID = 0;
	uint8 wTemp;

//	sciEnableNotification(BQ_UART, SCI_RX_INT);
//	rtiEnableNotification(rtiNOTIFICATION_COMPARE1);
	 /* rtiNOTIFICATION_COMPARE0 = 1ms
	 *  rtiNOTIFICATION_COMPARE1 = 5ms
	 *  rtiNOTIFICATION_COMPARE2 = 8ms
	 *  rtiNOTIFICATION_COMPARE3 = 10ms
	 */

	// Wake all devices
	// The wake tone will awaken any device that is already in shutdown and the pwrdown will shutdown any device
	// that is already awake. The least number of times to sequence wake and pwrdown will be half the number of
	// boards to cover the worst case combination of boards already awake or shutdown.
	for(nDev_ID = 0; nDev_ID < BQ_NUMBER_OF_DEVICES>>1; nDev_ID++) {
		bq_SendFrame(nDev_ID, DEV_CTRL_REG, 0x40, 1, FRMWRT_ALL_NR);	// send out broadcast pwrdown command
		delayms(5); //~5ms
		WakePL455();
		delayms(5); //~5ms
	}

	// Mask Customer Checksum Fault bit
	bq_SendFrame(0, MASK_DEV_REG, 0x8000, 2, FRMWRT_ALL_NR); // clear all fault summary flags

	// Clear all faults
	bq_SendFrame(0, FAULT_SUM_REG, 0xFFC0, 2, FRMWRT_ALL_NR);		// clear all fault summary flags
	bq_SendFrame(0, STATUS_REG, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

	// Auto-address all boards (section 1.2.2)
	bq_SendFrame(0, DEVCONFIG_REG, 0x19, 1, FRMWRT_ALL_NR); // set auto-address mode on all boards
	bq_SendFrame(0, DEV_CTRL_REG, 0x08, 1, FRMWRT_ALL_NR); // enter auto address mode on all boards, the next write to this ID will be its address

	// Set addresses for all boards in daisy-chain (section 1.2.3)
	for (nDev_ID = 0; nDev_ID < BQ_NUMBER_OF_DEVICES; nDev_ID++)
	{
		bq_SendFrame(nDev_ID, ADDR_REG, nDev_ID, 1, FRMWRT_ALL_NR); // send address to each board
	}

	// Enable all communication interfaces on all boards in the stack (section 1.2.1)
	bq_SendFrame(0, COMCONFIG_REG, 0x10F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces on all boards in stack

	/* Change to final baud rate used in the application (set by BAUDRATE define in pl455.h).
	 * Up to this point, all communication is at 250Kb, as the COMM_RESET done at the initial
	 * startup resets the bq76PL455A-Q1 UART to 250Kb. */
	switch(BQ_BAUDRATE)
	{
	case 125000:
		bq_SendFrame(0, COMCONFIG_REG, 0x00F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
		delayms(1);
		sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
		break;
	case 250000:
		delayms(1);
		break;
	case 500000:
		bq_SendFrame(0, COMCONFIG_REG, 0x20F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
		delayms(1);
		sciSetBaudrate(BQ_UART, BQ_BAUDRATE);
		break;
	case 1000000:
		bq_SendFrame(0, COMCONFIG_REG, 0x30F8, 2, FRMWRT_ALL_NR);	// set communications baud rate and enable all interfaces
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
		ReadReg(nDev_ID, ADDR_REG, &wTemp, 1, 1); // 1ms timeout

		if(nRead == 0) // if nothing is read then this board doesn't exist
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
							nSent = bq_SendFrame(nDev_ID, 16, 0x0080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 250000:
							nSent = bq_SendFrame(nDev_ID, 16, 0x1080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 500000:
							nSent = bq_SendFrame(nDev_ID, 16, 0x2080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						case 1000000:
							nSent = bq_SendFrame(nDev_ID, 16, 0x3080, 2, FRMWRT_SGL_NR);	// enable only single-end comm port on board
							break;
						}
				}
				else // this is the top board of a stack (section 1.2.5)
				{
					switch(BQ_BAUDRATE)
					{
					case 125000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x0028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 250000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x1028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 500000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x2028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
						break;
					case 1000000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x3028, 2, FRMWRT_SGL_NR);	// enable only comm-low and fault-low for the top board
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
						nSent = bq_SendFrame(nDev_ID, 16, 0x00D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 250000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x10D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 500000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x20D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					case 1000000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x30D0, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high and single-end comm port on bottom board
						break;
					}
				}
				else // this is a middle board
				{
					switch(BQ_BAUDRATE)
					{
					case 125000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x0078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 250000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x1078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 500000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x2078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					case 1000000:
						nSent = bq_SendFrame(nDev_ID, 16, 0x3078, 2, FRMWRT_SGL_NR);	// enable comm-high, fault-high, comm-low and fault-low on all middle boards
						break;
					}
				}
			}
		}
	}

	// Clear all faults (section 1.2.7)
	nSent = bq_SendFrame(0, 82, 0xFFC0, 2, FRMWRT_ALL_NR); // clear all fault summary flags
	nSent = bq_SendFrame(0, 81, 0x38, 1, FRMWRT_ALL_NR); // clear fault flags in the system status register

	delayms(10);
}

void bqWakeup(void)
{
	// toggle wake signal
	gioSetBit(gioPORTA, 0, 0); // assert wake (active low)
	delayus(10);
	gioToggleBit(gioPORTA, 0); // deassert wake
}

bq_dev_sample_data_t bqSample(void)
{
	// To fill
	return 0;
}

void bqShutdown(void)
{

}

void bqResetStack(void)
{
	// To fill
}

void bqSaveConfig(void)
{
	// To fill
}

uint16 bqGetFaults(void)
{
	// To fill
	return 0;
}

/******************************************************************************/
/*                       Local functions declaration                          */
/******************************************************************************/


uint16 CRC16(uint8 *pBuf, uint16 nLen);

void CommClear(void)
{
	int baudRate;
	baudRate = sciREG->BRS;

	sciREG->GCR1 &= ~(1U << 7U); // put SCI into reset
	sciREG->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	sciREG->PIO3 &= ~(1U << 2U); // set output to low

	delayus(baudRate * 2); // ~= 1/BAUDRATE/16*(155+1)*1.01
	sciInit();
	sciSetBaudrate(sciREG, BQ_BAUDRATE);
}

void CommReset(void)
{
	sciREG->GCR1 &= ~(1U << 7U); // put SCI into reset
	sciREG->PIO0 &= ~(1U << 2U); // disable transmit function - now a GPIO
	sciREG->PIO3 &= ~(1U << 2U); // set output to low

	delayus(200); // should cover any possible baud rate
	sciInit();
	sciSetBaudrate(sciREG, BQ_BAUDRATE);
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

void bq_WriteAll(bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen);

void bq_WriteReg(uint8 bID, bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen)

void bq_ReadReg(uint8 bID, bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen, uint8 ByteToReturn, uint32 dwTimeOut);


void bq_SendFrame(uint8 bID, bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen, bq_write_type_t bWriteType)
{
	uint8 bPktLen = 0;
	uint8 sendFrames[16];
	uint8 * pBuf = sendFrames;
	uint16   wCRC;

	// Use the pBuf pointer to increment through the sendFrame structure
	*pBuf = (1 << 7) ||  ((uint8)bWriteType << 4) || (0 << 3) || (bLen - 1); // Initialisation frame
	*pBuf++ = (uint8)bID; // Device ID
	*pBuf++ = (uint8)wAddr; // Device Register

	switch(bLen) // Scan through data frames and add to sendsFrames as needed
	{
	case 1:
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 2:
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 3:
		*pBuf++ = (dwData & 0x0000000000FF0000) >> 16;
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 4:
		*pBuf++ = (dwData & 0x00000000FF000000) >> 24;
		*pBuf++ = (dwData & 0x0000000000FF0000) >> 16;
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 5:
		*pBuf++ = (dwData & 0x000000FF00000000) >> 32;
		*pBuf++ = (dwData & 0x00000000FF000000) >> 24;
		*pBuf++ = (dwData & 0x0000000000FF0000) >> 16;
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 6:
		*pBuf++ = (dwData & 0x0000FF0000000000) >> 40;
		*pBuf++ = (dwData & 0x000000FF00000000) >> 32;
		*pBuf++ = (dwData & 0x00000000FF000000) >> 24;
		*pBuf++ = (dwData & 0x0000000000FF0000) >> 16;
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	case 8:
		*pBuf++ = (dwData & 0xFF00000000000000) >> 56;
		*pBuf++ = (dwData & 0x00FF000000000000) >> 48;
		*pBuf++ = (dwData & 0x0000FF0000000000) >> 40;
		*pBuf++ = (dwData & 0x000000FF00000000) >> 32;
		*pBuf++ = (dwData & 0x00000000FF000000) >> 24;
		*pBuf++ = (dwData & 0x0000000000FF0000) >> 16;
		*pBuf++ = (dwData & 0x000000000000FF00) >> 8;
		*pBuf++ =  dwData & 0x00000000000000FF;
		break;
	default:
		return;
	}

	bPktLen = pBuf - sendFrames;

	wCRC = CRC16(sendFrames, bPktLen);
	*pBuf++ = wCRC & 0x00FF;
	*pBuf++ = (wCRC & 0xFF00) >> 8;
	bPktLen += 2;

	sciSend(BQ_UART, bPktLen, sendFrames);
}


uint8 * bq_WaitResponse(uint8 ByteToReturn)
{
//	uint32 intFrame;
	uint8 dBuf[132];
	uint8 * pdBuf = dBuf;
	uint8 RxDataLen;
	uint8 CRC_response[2];
	uint16 CRC_check;

	*pdBuf = (uint8)(sciReceiveByte(BQ_UART) && 0x000000FF); // put int frame at start of buffer

	if(*pdBuf == (ByteToReturn - 1)){ // Check data length as expected
		sciReceive(BQ_UART, ByteToReturn, pdBuf++); // Read expected number of data frames into buffer plus 1 (because of int frame)
		sciReceive(BQ_UART, 2, CRC_response); // CRC response frames
	}
	else return 0; // if check fails return 0

	RxDataLen = pdBuf - dBuf; // Calculate length of buffered data for calculating CRC

	CRC_check = CRC16(pdBuf, RxDataLen);

	if ( (CRC_response[0] == (CRC_check & 0x00FF)) ||
		 (CRC_response[1] == (CRC_check & 0xFF00)) >> 8){
	}
	else return 0; // If CRC check fails return 0

	pdBuf = dBuf; // Reset Pointer to start of buffer again

	return (pdBuf +1); // If checks passed return pointer to data

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

