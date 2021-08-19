/*
 * =============================================================================
 *
 *                        Copyright 2002 by KVASER AB
 *            P.O Box 4076 S-51104 KINNAHULT, SWEDEN Tel. +46 320 15287
 *
 * This software is furnished under a license and may be used and copied
 * only in accordance with the terms of such license.
 *
 *
 * =============================================================================
*/

#ifndef KVADBLIB_H
#define KVADBLIB_H


#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    kvaDbOK                      =  0,
    kvaDbErr_Fail                = -1,
    kvaDbErr_NoDatabase          = -2,
    kvaDbErr_Param               = -3,
    kvaDbErr_NoMsg               = -4,
    kvaDbErr_NoSignal            = -5,
    kvaDbErr_Internal            = -6,
    kvaDbErr_DbFileOpen          = -7,
    kvaDbErr_DatabaseInternal    = -8,
    kvaDbErr_NoNode              = -9,
    kvaDbErr_NoAttrib            = -10,
    kvaDbErr_OnlyOneAllowed      = -11, /* Only one CANdb structure is allowed, me, 12.11.2008 */
} KvaDbStatus;

// Message flags qqq
#define KVADB_MESSAGE_EXT        0x80000000
#define KVADB_MESSAGE_J1939      0x00000001
#define KVADB_MESSAGE_WAKEUP     0x00000002

typedef enum {
  kvaDb_Intel     = 0,
  kvaDb_Motorola  = 1,
} KvaDbSignalEncoding;

typedef enum  {
   kvaDb_Invalid,
   kvaDb_Signed,
   kvaDb_Unsigned,
   kvaDb_Float,
   kvaDb_Double
} KvaDbSignalType;

// Database flags
#define KVADB_DATABASE_J1939      0x0001

typedef void* KvaDbHnd;
typedef void* KvaDbMessageHnd;
typedef void* KvaDbSignalHnd;
typedef void* KvaDbNodeHnd;


KvaDbStatus WINAPI kvaDbOpen(KvaDbHnd *dh);
KvaDbStatus WINAPI kvaDbAddFile(KvaDbHnd dh, const char *filename);
KvaDbStatus WINAPI kvaDbSetDummyFileName(KvaDbHnd dh, const char *filename);
KvaDbStatus WINAPI kvaDbClose(KvaDbHnd dh);

KvaDbStatus WINAPI kvaDbWriteFile(KvaDbHnd dh, char* filename);
KvaDbStatus WINAPI kvaDbReadFile(KvaDbHnd dh, char* filename);


KvaDbStatus WINAPI kvaDbGetFlags(KvaDbHnd dh, unsigned int *flags);

KvaDbStatus WINAPI kvaDbGetFirstMsg(KvaDbHnd dh, KvaDbMessageHnd *mh);
KvaDbStatus WINAPI kvaDbGetNextMsg(KvaDbHnd dh, KvaDbMessageHnd *mh);
KvaDbStatus WINAPI kvaDbGetMsgById(KvaDbHnd dh, unsigned int id, KvaDbMessageHnd *mh);
KvaDbStatus WINAPI kvaDbGetMsgByName(KvaDbHnd dh, const char *msg_name, KvaDbMessageHnd *mh);


KvaDbStatus WINAPI kvaDbGetMsgName(KvaDbMessageHnd mh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetMsgQualifiedName(KvaDbMessageHnd mh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetMsgComment(KvaDbMessageHnd mh, char *buf, size_t buflen);

// "flags" contains extended? or use 0x80000000 convention?
// could also contain J1939 flag, wakeup etc. qqq
KvaDbStatus WINAPI kvaDbGetMsgId(KvaDbMessageHnd mh, unsigned int *id, unsigned int *flags);
KvaDbStatus WINAPI kvaDbGetMsgDlc(KvaDbMessageHnd mh, int *dlc);

KvaDbStatus WINAPI kvaDbAddMsg(KvaDbHnd dh, KvaDbMessageHnd *mh);
KvaDbStatus WINAPI kvaDbDeleteMsg(KvaDbHnd dh, KvaDbMessageHnd mh);

KvaDbStatus WINAPI kvaDbSetFlags(KvaDbHnd dh, unsigned int flags);

KvaDbStatus WINAPI kvaDbSetMsgName(KvaDbMessageHnd mh, char *buf);
KvaDbStatus WINAPI kvaDbSetMsgComment(KvaDbMessageHnd mh, char *buf);

// "flags" contains extended? or use 0x80000000 convention?
// could also contain J1939 flag, wakeup etc. qqq
KvaDbStatus WINAPI kvaDbSetMsgId(KvaDbMessageHnd mh, unsigned int id, unsigned int flags);
KvaDbStatus WINAPI kvaDbSetMsgDlc(KvaDbMessageHnd mh, int dlc);


// qqq pragma för att sätta sizeof(enum) till 4? annars skippa enum...


// J1939 fungerar hur? Helt enkelt genom att id == PGN?  Hur hantera
// SA, DA, etc? qqq


KvaDbStatus WINAPI kvaDbGetFirstSignal(KvaDbMessageHnd mh, KvaDbSignalHnd *sh);
KvaDbStatus WINAPI kvaDbGetNextSignal(KvaDbMessageHnd mh, KvaDbSignalHnd *sh);

// Get a signal given its name and its message.
KvaDbStatus WINAPI kvaDbGetSignalByName(KvaDbMessageHnd mh, char *buf, KvaDbSignalHnd *sh);

// Get various values from a signal.
KvaDbStatus WINAPI kvaDbGetSignalValueFloat(KvaDbSignalHnd sh, double *f, void *data, size_t len);
KvaDbStatus WINAPI kvaDbGetSignalValueInteger(KvaDbSignalHnd sh, int *f, void *data, size_t len);

// Get various signal properties
KvaDbStatus WINAPI kvaDbGetSignalValueLimits(KvaDbSignalHnd sh, double *minval, double *maxval);
KvaDbStatus WINAPI kvaDbGetSignalValueScaling(KvaDbSignalHnd sh, double *factor, double *offset);
KvaDbStatus WINAPI kvaDbGetSignalValueSize(KvaDbSignalHnd sh, int *startbit, int *length);

KvaDbStatus WINAPI kvaDbGetSignalName(KvaDbSignalHnd sh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetSignalQualifiedName(KvaDbSignalHnd sh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetSignalComment(KvaDbSignalHnd sh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetSignalUnit(KvaDbSignalHnd sh, char *buf, size_t buflen);
KvaDbStatus WINAPI kvaDbGetSignalEncoding(KvaDbSignalHnd sh, KvaDbSignalEncoding *e);

KvaDbStatus WINAPI kvaDbGetSignalRepresentationType(KvaDbSignalHnd sh, KvaDbSignalType *t);
KvaDbStatus WINAPI kvaDbGetSignalPresentationType(KvaDbSignalHnd sh, KvaDbSignalType *t);

KvaDbStatus WINAPI kvaDbAddSignal(KvaDbMessageHnd mh, KvaDbSignalHnd *sh);
KvaDbStatus WINAPI kvaDbDeleteSignal(KvaDbMessageHnd mh, KvaDbSignalHnd sh);

KvaDbStatus WINAPI kvaDbSetSignalValueLimits(KvaDbSignalHnd sh, double minval, double maxval);
KvaDbStatus WINAPI kvaDbSetSignalValueScaling(KvaDbSignalHnd sh, double factor, double offset);
KvaDbStatus WINAPI kvaDbSetSignalValueSize(KvaDbSignalHnd sh, int startbit, int length);

KvaDbStatus WINAPI kvaDbSetSignalName(KvaDbSignalHnd sh, char *buf);
KvaDbStatus WINAPI kvaDbSetSignalComment(KvaDbSignalHnd sh, char *buf);
KvaDbStatus WINAPI kvaDbSetSignalUnit(KvaDbSignalHnd sh, char *buf);
KvaDbStatus WINAPI kvaDbSetSignalEncoding(KvaDbSignalHnd sh, KvaDbSignalEncoding e);

KvaDbStatus WINAPI kvaDbSetSignalRepresentationType(KvaDbSignalHnd sh, KvaDbSignalType t);

KvaDbStatus WINAPI kvaDbGetFirstNode(KvaDbHnd dh, KvaDbNodeHnd *nh);
KvaDbStatus WINAPI kvaDbGetNextNode(KvaDbHnd dh, KvaDbNodeHnd *nh);
KvaDbStatus WINAPI kvaDbGetNodeByName(KvaDbHnd dh, const char *node_name, KvaDbNodeHnd *nh);
KvaDbStatus WINAPI kvaDbGetNodeAttributeIntByName(KvaDbNodeHnd nh, const char *attrName, int *val);

KvaDbStatus WINAPI kvaDbStoreSignalValuePhys(KvaDbSignalHnd sh, unsigned char *can_data, int dlc, double value);
KvaDbStatus WINAPI kvaDbStoreSignalValueRaw(KvaDbSignalHnd sh, unsigned char *can_data, int dlc, int value);




#ifdef __cplusplus
}
#endif

#endif //KVADBLIB_H
