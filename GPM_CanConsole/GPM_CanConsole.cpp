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

#define MAX_FWSIZE (1500)
#define BLK_FWSIZE (0x100)

unsigned char fwfile[500000];

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
	memset(m_channelData.channel, 0, MAX_CHANNELS*sizeof(ChannelDataStruct));
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



int main(int argc, char* argv[])
{
	stMsg_T stMsg;
	long id;
	unsigned char cdata[10];
	DWORD time;
	unsigned int dlc, flags;
	unsigned int i, loop = 0;
	bool updating = false;
	bool nextBlock = false;
	unsigned short crc, fwstat = 0;
	int rtn, iFW, nFW, wFW;
	FILE* fw;

	if (argc == 2) if ((strcmp(argv[1],"--help")==0) || (strcmp(argv[1],"/?")==0)) {
		printf("Syntax: \r\n   GPM_CanConsole [<CAN channel>] [<firmware.bin>]\r\n");
		return 0;
	}

	if (InitDriver() == false) {
		canUnloadLibrary();
		printf("No KVaser adaptor found.\r\n");
		exit(-1);
	}

	if (argc > 1) {
		if (strlen(argv[argc - 1]) > 2) {
			// update firmware
			memset(fwfile, 255, sizeof(fwfile));
			errno_t nErr = fopen_s(&fw, argv[argc - 1], "rb");
			if (nErr == 0) {
				updating = true;
				nFW = fread(fwfile, 1, sizeof(fwfile), fw);
				nFW = (nFW / BLK_FWSIZE + 1) * BLK_FWSIZE;
				fclose(fw);
				crc = 0;
				for (int i = 0; i < nFW; i++) crc += fwfile[i];
				fw = NULL;
				iFW = 0;
				wFW = 0;
			}
		}
	}
	
	int stat = canBusOn(m_channelData.channel[0].hnd);

	if (updating)
		printf("Connected. Press <ESC> to quit. Press F2 to update again.\r\n");
	else
		printf("Connected. Press <ESC> to quit.\r\n");

	if (!updating) {
		stMsg.ArbIDOrHeader = CAN_MSG_CONSOLE;	// arbritration ID
		strcpy_s((char*)&stMsg.Data[0], 8, "?v\r");
		stMsg.NumberBytesData = (char)strlen((char*)stMsg.Data);

		rtn = canWrite(m_DriverConfig->channel[0].hnd,
			stMsg.ArbIDOrHeader,
			&stMsg.Data[0],
			stMsg.NumberBytesData,
			canMSG_STD);
	}

	while (true) {

		if (_kbhit()) {
			int key = _getch();
			if (key == 0) {
				key = _getch();
				if (key == 0x3C) { // F2 key
					
					memset(fwfile, 255, sizeof(fwfile));
					errno_t nErr = fopen_s(&fw, argv[argc - 1], "rb");
					if (nErr == 0) {
						updating = true;
						nFW = fread(fwfile, 1, sizeof(fwfile), fw);
						nFW = (nFW / BLK_FWSIZE + 1) * BLK_FWSIZE;
						fclose(fw);
						crc = 0;
						for (int i = 0; i < nFW; i++) crc += fwfile[i];
						fw = NULL;
						iFW = 0;
						wFW = 0;
					}
				}
			}
			else if (key == 27) break;
			else {
				stMsg.ArbIDOrHeader = CAN_MSG_CONSOLE;	// arbritration ID
				stMsg.Data[0] = key;
				stMsg.NumberBytesData = 1;

				rtn = canWrite(m_DriverConfig->channel[0].hnd,
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
			if (id == CAN_MSG_FIRMWARE) {
				fwstat = cdata[0];
				int newFW = (cdata[1] * 0x10000 + cdata[2] * 0x100 + cdata[3]);
				if ((newFW == 0) && (iFW > 0)) {
					printf("Failed.\r\n");
					updating = false;
				}
				else
					iFW = newFW;
			}
		}

		// update firmware
		if (updating) {
			if ((fwstat > 0) || (iFW == 0)) {
				if (iFW >= nFW) {
					// finished
					cdata[0] = 0x55;
					cdata[1] = 0xAA;
					cdata[2] = 0xFF;
					cdata[3] = 0xFF;
					cdata[4] = crc >> 8;
					cdata[5] = crc & 0xFF;
					printf("\r\nUpdate complete. CRC=%i\r\n",crc);
					updating = false;
				}
				else {
					cdata[0] = 0x55;
					cdata[1] = 0xAA;
					cdata[2] = (iFW / BLK_FWSIZE) >> 8;
					cdata[3] = (iFW / BLK_FWSIZE) & 0xFF;
					printf("\rFirmware: 0x%08X  (%i%%)", iFW, iFW * 100 / nFW);
				}
				rtn = canWrite(m_DriverConfig->channel[0].hnd,
					CAN_MSG_FIRMWARE, &cdata, 6, canMSG_STD);
				Sleep(2);
				if (updating) {
					// send 64 messages
					for (int ifw = 0; ifw < BLK_FWSIZE; ifw += 8) {
						rtn = canWrite(m_DriverConfig->channel[0].hnd,
							CAN_MSG_FIRMWARE, &fwfile[iFW + ifw], 8, canMSG_STD);
						Sleep(1);
					}
					if (iFW == 0) iFW = -1;
					fwstat = 0;
				}
			}
			else {
				// after timeout, re-transmit firmware packet
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

