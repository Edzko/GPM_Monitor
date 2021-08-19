// GPM_CanConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include <canlib.h>
#include <conio.h>

#define MAX_CHANNELS 3
#define CAN_MSG_CONSOLE 0x7E0
#define CAN_MSG_FIRMWARE 0x7E1
#define CAN_MSG_TERMINAL 0x7E2

typedef struct
{
	unsigned int ArbIDOrHeader;
	char NumberBytesData;
	unsigned char Data[8];
} stMsg_T;

typedef struct ChannelDataStruct_T {
	int        channel;
	char       name[100];
	DWORD      hwType;
	canHandle  hnd;
	int        hwIndex;
	int        hwChannel;
	int        isOnBus;
	int        driverMode;
	int        txAck;
} ChannelDataStruct;

typedef struct driverData_T {
	unsigned int       channelCount;
	ChannelDataStruct  channel[MAX_CHANNELS];
} driverData;

driverData m_channelData;
driverData* m_DriverConfig = &m_channelData;

bool InitDriver(void)
{
	int  i;
	int  stat;
	bool found = false;

	// Initialize ChannelData.
	memset(m_channelData.channel, 0, sizeof(m_channelData));
	for (i = 0; i < MAX_CHANNELS; i++) {
		m_channelData.channel[i].isOnBus = 0;
		m_channelData.channel[i].driverMode = canDRIVER_NORMAL;
		m_channelData.channel[i].channel = -1;
		m_channelData.channel[i].hnd = -1;
		m_channelData.channel[i].txAck = 0; // Default is TxAck off
	}
	m_channelData.channelCount = MAX_CHANNELS;


	//initialize CANlib
	canInitializeLibrary();

	//get number of present channels
	stat = canGetNumberOfChannels((int*)&m_channelData.channelCount);

	for (i = 0; i < (int)m_channelData.channelCount; i++)
	{
		canHandle  hnd;

		//obtain some hardware info from CANlib
		m_channelData.channel[i].channel = i;
		canGetChannelData(i, canCHANNELDATA_CHANNEL_NAME, m_channelData.channel[i].name,
			sizeof(m_channelData.channel[i].name));
		canGetChannelData(i, canCHANNELDATA_CARD_TYPE, &m_channelData.channel[i].hwType,
			sizeof(DWORD));

		//open CAN channel
		hnd = canOpenChannel(i, canOPEN_ACCEPT_VIRTUAL);
		if (hnd < 0) {
			// error
			//PRINTF_ERR(("ERROR canOpenChannel() in initDriver() FAILED Err= %d. <line: %d>\n",
			//		hnd, __LINE__));
		}
		else
		{
			m_channelData.channel[i].hnd = hnd;
			if ((stat = canIoCtl(hnd, canIOCTL_FLUSH_TX_BUFFER, NULL, NULL)) != canOK)
			{
				//PRINTF_ERR(("ERROR canIoCtl(canIOCTL_FLUSH_TX_BUFFER) FAILED, Err= %d <line: %d>\n",
				//		stat, __LINE__));
			}
		}

		//set the channels busparameters
		stat = canSetBusParams(hnd, BAUD_500K, 0, 0, 0, 0, 0);

		if (m_channelData.channel[i].hwType == 48) found = true;
	}
	return found;
}



int main()
{
	stMsg_T stMsg;
	long id;
	unsigned char cdata[10];
	DWORD time;
	unsigned int dlc, flags;
	unsigned int i, loop = 0;

	if (InitDriver() == false) {
		canUnloadLibrary();
		exit(-1);
	}

	int stat = canBusOn(m_channelData.channel[0].hnd);

	while (true) {

		if (_kbhit()) {
			int key = _getch();
			if (key == 27) break;
			else {
				stMsg.ArbIDOrHeader = CAN_MSG_CONSOLE;	// arbritration ID
				stMsg.Data[0] = key;
				stMsg.NumberBytesData = 1;

				int rtn = canWrite(m_DriverConfig->channel[0].hnd,
					stMsg.ArbIDOrHeader,
					&stMsg.Data[0],
					stMsg.NumberBytesData,
					canMSG_STD);
			}
		}

		while (canRead(m_channelData.channel[0].hnd, &id, &cdata[0], &dlc, &flags, &time) == canOK) {
			if (id == CAN_MSG_TERMINAL) {
				cdata[dlc] = 0;
				printf("%s", cdata);
			}
		}
		Sleep(1);
	}
	
	for (i = 0; i < m_channelData.channelCount; i++) {
		canStatus stat;
		stat = canBusOff(m_channelData.channel[i].hnd);
		stat = canClose(m_channelData.channel[i].hnd);
	}
	

	canUnloadLibrary();
	exit(0);
}

