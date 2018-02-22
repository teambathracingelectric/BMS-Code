/** @file sys_main.c 
*   @brief Application main file
*   @date 07-July-2017
*   @version 04.07.00
*
*   This file contains an empty main function,
*   which can be used for the application.
*/

/* 
* Copyright (C) 2009-2016 Texas Instruments Incorporated - www.ti.com 
* 
* 
*  Redistribution and use in source and binary forms, with or without 
*  modification, are permitted provided that the following conditions 
*  are met:
*
*    Redistributions of source code must retain the above copyright 
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the 
*    documentation and/or other materials provided with the   
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/


/* USER CODE BEGIN (0) */
//#include "gio.h"
#include "sci.h"
//#include "rti.h"
//#include "sys_vim.h"
//#include "pl455.h"
/* USER CODE END */

/* Include Files */

#include "sys_common.h"

/* USER CODE BEGIN (1) */
//#define  TSIZE1 10
//uint8  TEXT1[TSIZE1]= {'H','E','R','C','U','L','E','S',' ',' '};
//#define  TSIZE2 18
//uint8  TEXT2[TSIZE2]= {'M','I','C','R','O','C','O','N','T','R','O','L','L','E','R','S',' ',' '};
//#define  TSIZE3 19
//uint8  TEXT3[TSIZE3]= {'T','E','X','A','S',' ','I','N','S','T','R','U','M','E','N','T','S','\n','\r'};

static uint8 command;

//void sciDisplayText(sciBASE_t *sci, uint8 *text, uint32 length);
//void wait(uint32 time);

//#define SLAVE_UART sciREG
/* USER CODE END */

/** @fn void main(void)
*   @brief Application main function
*   @note This function is empty by default.
*
*   This function is called after startup.
*   The user can use this function to implement the application.
*/

/* USER CODE BEGIN (2) */
//int UART_RX_RDY = 0;
//int RTI_TIMEOUT = 0;
/* USER CODE END */

int main(void)
{
/* USER CODE BEGIN (3) */
	// Initialise Variables
//	uint8 bFrame[132];

	_enable_IRQ();

	// Main slave call function structure
//	pl455_main_t pl455;

	// Initialise based on HalCoGen configuration
//	gioInit();
	sciInit();
//	sciSetBaudrate(sciREG, BAUDRATE);
//	rtiInit();
//	vimInit();

//	sciEnableNotification

//	pl455 =  pl455_ctor();

	sciSend(sciREG, 21, (uint8 *)"Please press a key!\r\n");

	sciReceive(sciREG, 1, (uint8 *)&command);

	sciSend(scilinREG, 21, (uint8 *)"Please press a key!\r\n");

	sciReceive(scilinREG, 1, (uint8 *)&command);

	while(1)        /* continious desplay        */
	{
//		pl455.ForceWakeup();
//		sciDisplayText(UART,&TEXT1[0],TSIZE1);   /* send text code 1 */
//		sciDisplayText(UART,&TEXT2[0],TSIZE2);   /* send text code 2 */
//		sciDisplayText(UART,&TEXT3[0],TSIZE3);   /* send text code 3 */
//		wait(200);

//		pl455.BroadcastSampleandSend(pl455.bFrames);


	};
/* USER CODE END */

    return 0;
}


/* USER CODE BEGIN (4) */
//void sciDisplayText(sciBASE_t *sci, uint8 *text,uint32 length)
//{
//    while(length--)
//    {
//        while ((sci->FLR & 0x4) == 4); /* wait until busy */
//        sciSendByte(sci,*text++);      /* send out text   */
//    };
//}
//
//
//void wait(uint32 time)
//{
//    time--;
//}

void sciNotification(sciBASE_t *sci, unsigned flags)
{
	/* Echo received character */
	sciSend(sci, 1, (uint8 *)&command);

	/* Await further character */
	sciReceive(sci, 1, (uint8 *)&command);
}
/* USER CODE END */
