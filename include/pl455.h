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

// Define pl455 globals defines
#define TOTALBOARDS	16
#define BAUDRATE 250000

// Define pl455 global functions
void delayms(uint16 ms);
void delayus(uint16 us);

class pl455_c
{
	public:
		// Define public call functions
		void Reset(void);
		void Wakeup(void);
		void CommClear(void);
		void CommReset(void);
		boolean GetFaultStat(void);
//		void ForceWakeup(void);
//		void PowerDown(void);
//		void MaskCheckSumFault(void);
//		void ClearAllFaults(void);
//		void AutoAddress(void);
//		void EnableComs(void);
//		void SetSciBaudrate(uint32 baudrate);
//		void ConfigureStackComms(void);
//		void SetMuxDelay(uint16 delay);
//		void SetInternalSamplePeriod(uint16 period);
//		void SetOversampleRate(uint16 rate);
//		boolean ClearCheckFaults(void);

	protected:
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

		// Private functions
		uint16 B2SWORD(uint16 wIN);
		uint32 B2SDWORD(uint32 dwIN);
		uint32 B2SINT24(uint32 dwIN24);

		uint32  WriteReg(uint8 bID, uint16 wAddr, uint64 dwData, uint8 bLen, uint8 bWriteType);
		uint32  ReadReg(uint8 bID, uint16 wAddr, void * pData, uint8 bLen, uint32 dwTimeOut);

		uint32  WriteFrame(uint8 bID, uint16 wAddr, uint8 * pData, uint8 bLen, uint8 bWriteType);
		uint32  ReadFrameReq(uint8 bID, uint16 wAddr, uint8 buint8ToReturn);
		uint32  WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut);


	} pl455;

#endif /* PL455_H_ */
//EOF
