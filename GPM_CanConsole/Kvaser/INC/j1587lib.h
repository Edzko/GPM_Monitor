/*
**                   Copyright 2007 by KVASER AB, SWEDEN      
**                        WWW: http://www.kvaser.com
**
** This software is furnished under a license and may be used and copied
** only in accordance with the terms of such license.
**
*/

#ifndef __J1587LIB_H
#define __J1587LIB_H

#include <windows.h>


typedef int J1587Handle;
#define j1587INVALID_HANDLE      (J1587Handle)(-1))


/* K-Line CAN-IDs */
#define CAN_ID_FLAG_WAKEUP      0x004
#define CAN_ID_FLAG_CS          0x008

#define CAN_ID_ISO_DOWN_TX      0x780 /* DRVlin RX Id for command and data from host */
#define CAN_ID_ISO_DOWN_RX      0x781 /* DRVlin TX Id for flow control */

#define CAN_ID_ISO_UP_RX        0x770 /* DRVlin TX Id for response to host */
#define CAN_ID_ISO_UP_TX        0x771 /* DRVlin RX Id for flow control from host - not used yet */

#define CAN_ID_TX_SOF           0x7e0
#define CAN_ID_TX_SOFW          (CAN_ID_TX_SOF | CAN_ID_FLAG_WAKEUP)
#define CAN_ID_TX_FF            0x7e1

#define CAN_ID_TX_CS_SOF        (CAN_ID_TX_SOF  | CAN_ID_FLAG_CS)
#define CAN_ID_TX_CS_SOFW       (CAN_ID_TX_SOFW | CAN_ID_FLAG_CS)
#define CAN_ID_TX_CS_FF         (CAN_ID_TX_FF   | CAN_ID_FLAG_CS)

#define CAN_ID_RX_SOF           0x7f0
#define CAN_ID_RX_FF            0x7f1
#define CAN_ID_RX_STATUS        0x7f8

#define CAN_ID_ACTIVATED        0x760

// Linx tranceiver types
#define canTRANSCEIVER_TYPE_LINX_LIN       64
#define canTRANSCEIVER_TYPE_LINX_J1708     66
#define canTRANSCEIVER_TYPE_LINX_K         68
#define canTRANSCEIVER_TYPE_LINX_SWC       70
#define canTRANSCEIVER_TYPE_LINX_LS        72

/* Operation modes */
#define EEPROM_OP_MODE_NONE             0x00

#define EEPROM_OP_MODE_LIN_MASTER       0x01
#define EEPROM_OP_MODE_LIN_SLAVE        0x02
//#define EEPROM_OP_MODE_LIN_PASS_THRU    0x03

#define EEPROM_OP_MODE_J1587_NORMAL     0x01
#define EEPROM_OP_MODE_J1587_NODE       0x02

/* Flags for J1587MessageInfo */
#define j1587FLAG_CHECKSUM              0x80
#define j1587FLAG_FRAMEDELAY            0x40
#define j1587FLAG_BYTEDELAY             0x20
#define j1587FLAG_STOPBIT               0x10
#define j1587FLAG_OVERRUN               0x08

/** Return codes from the J1587lib functions.*/
typedef enum {
  j1587OK                  = 0,
  j1587ERR_NOMSG           = -1,     //< No messages available
  j1587ERR_NOTRUNNING      = -3,
  j1587ERR_RUNNING         = -4,
  j1587ERR_NORMALONLY      = -5,
  j1587ERR_NODEONLY        = -6,
  j1587ERR_PARAM           = -7,     //< Error in parameter
  j1587ERR_NOTFOUND        = -8,     //< Specified hw not found
  j1587ERR_NOMEM           = -9,     //< Out of memory
  j1587ERR_NOCHANNELS      = -10,    //< No channels avaliable
  j1587ERR_TIMEOUT         = -11,    //< Timeout occurred
  j1587ERR_NOTINITIALIZED  = -12,    //< Lib not initialized
  j1587ERR_NOHANDLES       = -13,    //< Can't get handle
  j1587ERR_INVHANDLE       = -14,    //< Handle is invalid
  j1587ERR_CANERROR        = -15,
  j1587ERR_ERRRESP         = -16,    //< There was an error response from the J1587 interface
  j1587ERR_WRONGRESP       = -17,    //< The J1587 interface response wasn't the expected one
  j1587ERR_DRIVER          = -18,    //< CAN driver type not supported
  j1587ERR_DRIVERFAILED    = -19,    //< DeviceIOControl failed; use Win32 GetLastError()
  j1587ERR_NOCARD          = -20,    //< The card was removed or not inserted
  j1587ERR_LICENSE         = -21,    //< The license is not valid.
  j1587ERR_INTERNAL        = -22,    //< Internal error in the driver.
  j1587ERR_NO_ACCESS       = -23,    //< Access denied
  j1587ERR_VERSION         = -24     //< Function not supported in this version.
} J1587Status;


#define J1587LIB_VERSION  1

// sizeof(J1587MessageInfo) should be 16
#include <pshpack1.h>
typedef struct {
  // To enable future extensions of this structure (see J1587LIB_VERSION).
  unsigned char version;

  // Miscellaneous flags
  unsigned char flags;

  // Frame delay in bit times (up to 255).
  // Read:  Length of inter-frame delay.
  // Write: Delay before message transmission could begin.
  unsigned char frameDelay;

  union {
    // Checksum as read from the J1587 bus. Might not
    // match the data in case of J1587_CSUM_ERROR.
    unsigned char checkSum;
    // Number of retries before message could be transmitted.
    unsigned char retries;
  };

  // Timestamp in ms of the reception/transmission completion
  // of the checksum. Uses the canlib CAN timer.
  unsigned long timestamp;

  // Total frame length in microseconds;
  // from the first start bit to the end of the checksum.
  unsigned long frameLength;

  // Bitrate of the message in bits per second.
  // Range [1000 .. 60000] (plus some margin)
  unsigned short bitrate;

  // Reserved
  unsigned short reserved;
} J1587MessageInfo;
#include <poppack.h>


// Define J1587LIBAPI unless it's done already.
// (j1587lib.c provides its own definition of J1587LIBAPI before including this file.)
//
#ifndef J1587LIBAPI
#   if defined(__BORLANDC__)
#      define J1587LIBAPI __stdcall
#   elif defined(_MSC_VER) || defined(__MWERKS__) || defined(__GNUC__)
#      define J1587LIBAPI __stdcall
#   endif
#endif

#ifdef __cplusplus
extern "C" {
#endif

/** To be called before the library is used.
*/
void J1587LIBAPI j1587InitializeLibrary(void);


/** Open a channel to a J1587 interface.
* The J1587 cable must be powered.
* 
* The channel number is the same that
* is used by canOpenChannel() to access the LAPcan channel.
*/
J1587Handle J1587LIBAPI j1587OpenChannel(int channel, int flags);
// Flags for j1587OpenChannel:
#define J1587_NORMAL 1
#define J1587_NODE   2
#define J1587_READ   4
#define J1587_WRITE  8

/** Close the J1587 channel.
*/
J1587Status J1587LIBAPI j1587Close(J1587Handle h);

/* Get firmware version of the J1587 Interface.
* The version numbers aren't valid until j1587BusOn() has been called.
*/
J1587Status J1587LIBAPI j1587GetFirmwareVersion(J1587Handle h,
                                          unsigned char *bootVerMajor,
                                          unsigned char *bootVerMinor,
                                          unsigned char *bootVerBuild,
                                          unsigned char *appVerMajor,
                                          unsigned char *appVerMinor,
                                          unsigned char *appVerBuild);

/** Set the bit rate.
* The J1587 Interface should not be on bus.
*/
J1587Status J1587LIBAPI j1587SetBitrate(J1587Handle h, unsigned int bps);

/** Activate the J1587 interface.
* It will be reset, so any setups done earlier will be lost.
*/
J1587Status J1587LIBAPI j1587BusOn(J1587Handle h);

/** Deactivate the J1587 interface. It will be reset clearing any settings
* such as J1587 data buffers active.
*/
J1587Status J1587LIBAPI j1587BusOff(J1587Handle h);

/** Return the current timer value (used for timestamps)
*/
unsigned long J1587LIBAPI j1587ReadTimer(J1587Handle h);

/** Write a J1587 message. Times out after timeout ms.
*/
J1587Status J1587LIBAPI j1587WriteMessageWait(J1587Handle h, const void *msg,
                                              unsigned int length,
                                              unsigned int priority,
                                              DWORD timeout);

/** If a message is received, j1587OK is returned. Times out after
* timeout ms.  Returns j1587ERR_NOMSG if nothing is received.
*/
J1587Status J1587LIBAPI j1587ReadMessageWait(J1587Handle h, void *msg,
                                             unsigned int *msgLength,
                                             J1587MessageInfo *msgInfo,
                                             DWORD timeout);

/** Call this function to make sure all messages transmitted to the
* J1587 Interface have been received by it.
*
* When messages are transmitted to the J1587 Interface, they are queued by
* Windows before appearing on the CAN bus.
*
* The function returns j1587OK if all writes are done, j1587ERR_TIMEOUT in
* case of timeout or possibly some other error code.
*/
J1587Status J1587LIBAPI j1587WriteSync (J1587Handle h,
                                        J1587MessageInfo *msgInfo,
                                        DWORD timeout);

/** With this function, it is possible to change settings on a J1587 Interface
* that is on-bus.
* When going on-bus, the bitrate and the flag values listed here are set to the
* default value (either as hard-coded in the firmware, or as stored in the
* non-voilatile memory of the J1587 Interface).
*
* The J1587 Interface must be on bus for this command to work.
*/
J1587Status J1587LIBAPI j1587Configure(J1587Handle h, unsigned int flags);
#define J1587_INTER_CHAR_DELAY_MASK 0x0f
#define J1587_REPORT_BAD_CHECKSUM   0x10 // Default off
#define J1587_REPORT_FRAME_DELAY    0x20 // Default off
#define J1587_REPORT_CHAR_DELAY     0x40 // Default off

// Return the CAN handle given an open J1587 handle
J1587Status J1587LIBAPI j1587GetCanHandle(J1587Handle h, unsigned int *canHandle);

#ifdef __cplusplus
}
#endif

#endif // __J1587LIB_H
