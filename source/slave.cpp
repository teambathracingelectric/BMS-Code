/*
 * hal_slave.cpp
 *
 *  Created on: 8 Dec 2017
 *      Author: emf3d
 */

#include <slave.h>
#include "gio.h"
#include "sci.h"
#include "rti.h"
#include "sys_vim.h"
#include "pl455.h"

/*
Wakeup
turn on downstream coms
place in auto-address mode
send out new addresses, starting at zero
read back the value stored in the device address register - last device to respond is last device in chain
turnoff high side comms on highest device
turn off single ended comms on all but lowest
*/

/* Auto address slave IC stack
 *
 */
void slave_AddressStack(void)
{
	// initialize local variables
	int nSent, nRead, nTopFound = 0;
	int nDev_ID, nGrp_ID;
	BYTE  bFrame[132];
	uint32  wTemp = 0;

	/** Code examples
	 * The command sequences below are examples of the message examples in the bq76PL455 Communication Examples document.
	 * each message example references the section in the document.
	 */

	// Wake all devices
	// The wake tone will awaken any device that is already in shutdown and the pwrdown will shutdown any device
	// that is already awake. The least number of times to sequence wake and pwrdown will be half the number of
	// boards to cover the worst case combination of boards already awake or shutdown.
	for(nDev_ID = 0; nDev_ID < TOTALBOARDS>>1; nDev_ID++) {
		nSent = WriteReg(nDev_ID, 12, 0x40, 1, FRMWRT_ALL_NR);	// send out broadcast pwrdown command
		delayms(5); //~5ms
		WakePL455();
		delayms(5); //~5ms
	}
}

void enableDownstreamComs(void)
{

}
