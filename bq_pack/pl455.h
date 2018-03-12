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

/* General Config */
#define BQ_NUMBER_OF_DEVICES			1
#define BQ_BAUDRATE 					250000
#define BQ_UART							(sciBASE_t *)sciREG

//// AFE Config ////
#define BQ_SAMPLE_DELAY 					0 //ms
#define BQ_SAMPLE_PERIOD					0 // PLEASE SET!
#define BQ_OVERSAMPLE_RATE					0 // PLEASE SET!
#define BQ_FAULT_MASK						0 // PLEASE SET!
#define BQ_OVERVOLTAGE_THRESHOLD 			4500 //mV
#define BQ_UNDERVOLTAGE_THRESHOLD			2000 //mV
// Note: selected channels must match "bq_dev_sample_data_t" structure
#define BQ_VOLTAGE_CHANNEL_SELECT			0xFFFF // 7.6.3.3: All voltage sense channels, bit 1 = cell 1
#define BQ_AUX_CHANNEL_SELECT				0xFF // 7.6.3.3: All Auxiliary Channels
#define BQ_DIGITAL_TEMP_SENSE_EN			1 // 7.6.3.3: Digital die temperature
#define BQ_ANALOGUE_TEMP_SENSE_EN			1 // 7.6.3.3: Analogue die temperature
#define BQ_ANALOGUE_VREF_SENSE_EN			0 // 7.6.3.3: 4.5v analogue voltage reference
#define BQ_VMODULE_SELECT					0 // 7.6.3.3: Sum-of-cells (VMODULE) monitor, note configure TSTCONFIG[MODULE_MON_EN]
#define BQ_VMMONSEL_EN						0 // 7.6.3.3: VM (negative supply charge pump) voltage monitor


/* BQ76PL455A Registers + lengths*/
typedef enum BQ_DEV_REGS
{
	SREV_REG 		= 0x00, // Silicon Revision - R - length: 2
	CMD_REG 		= 0x02, // Command - W - length: 1
	CHANNELS_REG	= 0x03, // Command channel select - R/W - length: 4
	OVERSMPL_REG	= 0x07, // Command averaging (oversampling) - R/W - length: 1
	ADDR_REG		= 0x0A, // Device address - R/W - length: 1
	GROUP_ID_REG	= 0x0B, // (Device) Group Identifier - R/W - length: 1
	DEV_CTRL_REG	= 0x0C, // Device control - R/W - length: 1
	NCHAN_REG		= 0x0D, // Number of channels enabled for conversion - R/W - length: 1
	DEVCONFIG_REG	= 0x0E, // Device configuration - R/W - length: 1
	PWRCONFIG_REG	= 0x0F, // Power configuration - R/W - length: 1
	COMCONFIG_REG	= 0x10, // Communications configuration - R/W - length: 2
	TXHOLDOFF_REG	= 0x12, // UART Transmitter holdoff - R/W - length: 1
	CBCONFIG_REG	= 0x13, // Cell balancing (equalization) configuration - R/W - length: 1
	CBENBL_REG		= 0x14, // Cell balancing enables - R/W - length: 2
	TSTCONFIG_REG	= 0x1E, // Built-In Self-Test (BIST) configuration - R/W - length: 2
	TESTCTRL_REG	= 0x20, // BIST control - R/W - length: 2
	TEST_ADC_REG	= 0x22, // ADC BIST control - R/W - length: 3
	TESTAUXPU_REG	= 0x25, // Test control—AUX pull-up resistors - R/W - length: 1
	CTO_REG			= 0x28, // Communications time-out - R/W - length: 1
	CTO_CNT_REG		= 0x29, // Communications time-out counter - R/W - length: 3
	AM_PER_REG		= 0x32, // Auto-monitor period - R/W - length: 1
	AM_CHAN_REG		= 0x33, // Auto-monitor channel select - R/W - length: 4
	AM_OSMPL_REG	= 0x37, // Auto-monitor averaging - R/W - length: 1
	SMPL_DLY1_REG	= 0x3D, // Initial sampling delay - R/W - length: 1
	CELL_SPER_REG	= 0x3E, // Cell and die temperature measurement period - R/W - length: 1
	AUX_SPER_REG	= 0x3F, // AUX channels sampling period - R/W - length: 4
	TEST_SPER_REG	= 0x43, // ADC test sampling period - R/W - length: 2
	SHDN_STS_REG	= 0x50, // Shutdown recovery status - R - length: 1
	STATUS_REG		= 0x51, // Device status - R/W - length: 1
	FAULT_SUM_REG	= 0x52, // Fault summary - R/W - length: 4
	FAULT_UV_REG	= 0x54, // Undervoltage faults - R/W - length: 4
	FAULT_OV_REG	= 0x56, // Overvoltage faults - R/W - length: 4
	FAULT_AUX_REG	= 0x58, // AUX threshold exceeded faults - R/W - length: 4
	FAULT_2UV_REG	= 0x5A, // Comparator UV faults - R/W - length: 4
	FAULT_2OV_REG	= 0x5C, // Comparator OV faults - R/W - length: 4
	FAULT_COM_REG	= 0x5E, // Communication faults - R/W - length: 4
	FAULT_SYS_REG	= 0x60, // System fault - R/W - length: 1
	FAULT_DEV_REG	= 0x61, // Device fault - R/W - length: 2
	FAULT_GPI_REG	= 0x63, // General purpose input (GPIO) fault - R/W - length: 1
	MASK_COMM_REG	= 0x68, // Communications FAULT mask register - R/W - length: 2
	MASK_SYS_REG	= 0x6A, // System FAULT mask register - R/W - length: 1
	MASK_DEV_REG	= 0x6B, // Chip FAULT mask register - R/W - length: 2
	FO_CTRL_REG		= 0x6E, // FAULT output control - R/W - length: 2
	GPIO_DIR_REG	= 0x78, // GPIO direction control - R/W - length: 1
	GPIO_OUT_REG	= 0x79, // GPIO output control - R/W - length: 1
	GPIO_PU_REG		= 0x7A, // GPIO pull-up resistor control - R/W - length: 1
	GPIO_PD_REG		= 0x7B, // GPIO pull-down resistor control - R/W - length: 1
	GPIO_IN_REG		= 0x7C, // GPIO input value - R - length: 1
	GP_FLT_IN_REG	= 0x7D, // GPIO input 0/1 FAULT assertion state - R/W - length: 1
	MAGIC1_REG		= 0x82, // "Magic" value enables EEPROM write - W - length: 4
	COMP_UV_REG		= 0x8C, // Comparator undervoltage threshold - R/W - length: 1
	COMP_OV_REG		= 0x8D, // Comparator overvoltage threshold - R/W - length: 1
	CELL_UV_REG		= 0x8E, // Cell undervoltage threshold - R/W - length: 2
	CELL_OV_REG		= 0x90, // Cell overvoltage threshold - R/W - length: 2
	AUX0_UV_REG		= 0x92, // AUX0 undervoltage threshold - R/W - length: 2
	AUX0_OV_REG		= 0x94, // AUX0 overvoltage threshold - R/W - length: 2
	AUX1_UV_REG		= 0x96, // AUX1 undervoltage threshold - R/W - length: 2
	AUX1_OV_REG		= 0x98, // AUX1 overvoltage threshold - R/W - length: 2
	AUX2_UV_REG		= 0x9A, // AUX2 undervoltage threshold - R/W - length: 2
	AUX2_OV_REG		= 0x9C, // AUX2 overvoltage threshold - R/W - length: 2
	AUX3_UV_REG		= 0x9E, // AUX3 undervoltage threshold - R/W - length: 2
	AUX3_OV_REG		= 0xA0, // AUX3 overvoltage threshold - R/W - length: 2
	AUX4_UV_REG		= 0xA2, // AUX4 undervoltage threshold - R/W - length: 2
	AUX4_OV_REG		= 0xA4, // AUX4 overvoltage threshold - R/W - length: 2
	AUX5_UV_REG		= 0xA6, // AUX5 undervoltage threshold - R/W - length: 2
	AUX5_OV_REG		= 0xA8, // AUX5 overvoltage threshold - R/W - length: 2
	AUX6_UV_REG		= 0xAA, // AUX6 undervoltage threshold - R/W - length: 2
	AUX6_OV_REG		= 0xAC, // AUX6 overvoltage threshold - R/W - length: 2
	AUX7_UV_REG		= 0xAE, // AUX7 undervoltage threshold - R/W - length: 2
	AUX7_OV_REG		= 0xB0, // AUX7 overvoltage threshold - R/W - length: 2
	LOT_NUM_REG		= 0xBE, // Device Lot Number -  R - length: 8
	SER_NUM_REG		= 0xC6, // Device Serial Number - R - length: 2
	SCRATCH_REG		= 0xC8, // User-defined data - R/W - length: 8
	VSOFFSET_REG	= 0xD2, // ADC voltage offset correction - R/W - length: 1
	VSGAIN_REG		= 0xD3, // ADC voltage gain correction - R/W - length: 1
	AX0OFFSET_REG	= 0xD4, // AUX0 ADC offset correction - R/W - length: 2
	AX1OFFSET_REG	= 0xD6, // AUX1 ADC offset correction - R/W - length: 2
	AX2OFFSET_REG	= 0xD8, // AUX2 ADC offset correction - R/W - length: 2
	AX3OFFSET_REG	= 0xDA, // AUX3 ADC offset correction - R/W - length: 2
	AX4OFFSET_REG	= 0xDC, // AUX4 ADC offset correction - R/W - length: 2
	AX5OFFSET_REG	= 0xDE, // AUX5 ADC offset correction - R/W - length: 2
	AX6OFFSET_REG	= 0xE0, // AUX6 ADC offset correction - R/W - length: 2
	AX7OFFSET_REG	= 0xE2, // AUX7 ADC offset correction - R/W - length: 2
	TSTR_ECC_REG	= 0xE6, // ECC Test Results - R - length: 8
	CSUM_REG		= 0xF0, // Saved checksum value - R/W - length: 4
	CSUM_RSLT_REG	= 0xF4, // Checksum Readout - R - length: 4
	TEST_CSUM_REG	= 0xF8, // Checksum Test Result - R - length: 2
	EE_BURN_REG		= 0xFA, // EEPROM Burn Count; up-counter - R - length: 1
	MAGIC2_REG		= 0xFC // "Magic" value enables EEPROM write - W - length: 4
} bq_dev_regs_t;

/* Transmission type */
typedef enum BQ_WRITE_TYPE
{
	FRMWRT_SGL_R	= 0x00, // single device write with response
	FRMWRT_SGL_NR	= 0x10, // single device write without response
	FRMWRT_GRP_R	= 0x20, // group broadcast with response
	FRMWRT_GRP_NR	= 0x30, // group broadcast without response
	FRMWRT_ALL_R	= 0x60, // general broadcast with response
	FRMWRT_ALL_NR	= 0x70 // general broadcast without response
} bq_write_type_t;


typedef struct BQ_DEV_SAMPLE_DATA
{
	uint8 cell_voltage[16];
//	uint8 cell_temp[BQ_AUX_CHANNEL_SELECT];
	uint8 internal_digital_temp;
	uint8 internal_analogue_temp;
} bq_dev_sample_data_t;

typedef struct BQ_DEV_COMMANDS
{
	void * IntStack;
	void * Wakeup;
	bq_dev_sample_data_t * Sample;
	void * Shutdown;
	void * ResetStack;
	void * SaveConfig;
	uint16 * GetFaults;
} bq_dev_commands_t;

/******************************************************************************/
/*                       Global functions declaration                          */
/******************************************************************************/
void bq_InitialiseStack(void);
void bq_Wakeup(void);
void bq_Sample_SGL(uint8 bID, bq_dev_sample_data_t * data);
//void bq_Shutdown(void);
//void bq_ResetStack(void);
//void bq_SaveConfig(void);
//uint16 bq_GetFaults(void);

/******************************************************************************/
/*                       Local functions declaration                          */
/******************************************************************************/
void ResetPL455(void);
//void WakePL455(void);
void CommClear(void);
void CommReset(void);
boolean GetFaultStat(void);

uint16  B2SWORD(uint16 wIN);
uint32 B2SDWORD(uint32 dwIN);
uint32 B2SINT24(uint32 dwIN24);

// Use WriteReg to write to slaves in single and global modes
sint32  bq_WriteReg(uint8 bID, bq_dev_regs_t wAddr, uint64 dwData, uint8 bLen, bq_write_type_t bWriteType);
sint32  bq_WriteFrame(uint8 bID, bq_dev_regs_t wAddr, uint8 * pData, uint8 bLen, bq_write_type_t bWriteType);

// Use ReadReg to read individual Registers
sint32  bq_ReadReg(uint8 bID, bq_dev_regs_t wAddr, void * pData, uint8 bLen, uint32 dwTimeOut);
sint32  bq_ReadFrameReq(uint8 bID, bq_dev_regs_t wAddr, uint8 bByteToReturn);

// Useful if need to read from response data e.g. sample request responses
sint32  bq_WaitRespFrame(uint8 *pFrame, uint8 bLen, uint32 dwTimeOut);

void delayms(uint16 ms);
void delayus(uint16 us);

#endif /* PL455_H_ */
//EOF
