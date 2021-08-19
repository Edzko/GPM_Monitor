// GPM_CanConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include <canlib.h>

#define MAX_CHANNELS 1
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


	//
	// Enumerate all installed channels in the system and obtain their names
	// and hardware types.
	//

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
		hnd = canOpenChannel(i, canWANT_VIRTUAL);
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
		if (i == 0) stat = canSetBusParams(hnd, BAUD_500K, 0, 0, 0, 0, 0);

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

	if (InitDriver() == false) {
		canUnloadLibrary();
		exit(-1);
	}

	for (int i = 0; i < 100; i++) {
		stMsg.ArbIDOrHeader = 0x123;	// arbritration ID
		stMsg.Data[0] = i;
		stMsg.Data[1] = 1;
		stMsg.Data[2] = 2;
		stMsg.Data[3] = 3;
		stMsg.Data[4] = 4;
		stMsg.Data[5] = 5;
		stMsg.Data[6] = 6;
		stMsg.Data[7] = 7;
		stMsg.NumberBytesData = 8;

		canWrite(m_DriverConfig->channel[0].hnd,
			stMsg.ArbIDOrHeader,
			&stMsg.Data[0],
			stMsg.NumberBytesData,
			canMSG_STD);

		if (canRead(m_channelData.channel[0].hnd, &id, &cdata[0], &dlc, &flags, &time) == canOK) {
			// print characters
			printf("Recv %i: 0x%03X - %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
				i, id, cdata[0], cdata[1], cdata[2], cdata[3], cdata[4], cdata[5], cdata[6], cdata[7]);
		}

		Sleep(100);
	}


	canUnloadLibrary();
	exit(0);
}

