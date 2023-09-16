// GPM_CanConsole.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <stdint.h>
#include <canlib.h>
#include <conio.h>

#include "api/candle.h"

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

bool InitKvaserDriver(void)
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

void WideToChar(wchar_t* wt, char *t)
{
	int i = 0;
	while (wt[i] != 0) {
		t[i] = (char)wt[i];
		i++;
	}
	t[i] = 0;
}
candle_handle dev;
candle_frame_t cframe;

bool InitCandleDriver(void) {
	candle_list_handle clist;
	uint8_t num_interfaces;
	bool hResult = false;
	wchar_t *wpath;
	char path[1000];

	if (candle_list_scan(&clist)) {
		if (candle_list_length(clist, &num_interfaces)) {
			for (uint8_t i = 0; i < num_interfaces; i++) {
				if (candle_dev_get(clist, i, &dev)) {
					wpath = candle_dev_get_path(dev);
					WideToChar(wpath, path);
					printf("Candle Interface = %s\r\n", path);
					
					return true;
				}

			}
		}
		candle_list_free(clist);
	}
	return hResult;
}

stMsg_T stMsg;
long id;
unsigned char cdata[10];
//DWORD time;
unsigned int dlc, flags;
bool haveKvaser, haveCandleApi;

void sendCAN(int id, unsigned char dlc, unsigned char* data)
{
	bool rtn;
	if (haveKvaser) {
		stMsg.ArbIDOrHeader = id;	// arbritration ID
		memcpy_s(&stMsg.Data[0], 8, data, dlc);
		stMsg.NumberBytesData = dlc;

		rtn = canWrite(m_DriverConfig->channel[0].hnd,id,data,dlc,canMSG_STD);
	}
	if (haveCandleApi)
	{
		cframe.can_id = id;
		memcpy_s(cframe.data, 8, data, dlc);
		cframe.can_dlc = dlc;
		
		rtn = candle_frame_send(dev, 0, &cframe);
	}
}
bool readCAN()
{
	DWORD time;
	if (haveKvaser) {
		if (canRead(m_channelData.channel[0].hnd, &id, &cdata[0], &dlc, &flags, &time) != canOK) { return false; };
	}
	if (haveCandleApi) {
		if (candle_frame_read(dev, &cframe, 0) == false) { return false; };
		id = cframe.can_id;
		memcpy(cdata, cframe.data, 8);
		dlc = cframe.can_dlc;
	}
	return true;
}

int main(int argc, char* argv[])
{
	unsigned int i, loop = 0;
	bool updating = false;
	bool nextBlock = false;
	unsigned short crc, fwstat = 0;
	int rtn, iFW, nFW, wFW;
	FILE* fw;
	int stat;

	if (argc == 2) if ((strcmp(argv[1],"--help")==0) || (strcmp(argv[1],"/?")==0)) {
		printf("Syntax: \r\n   GPM_CanConsole [<CAN channel>] [<firmware.bin>]\r\n");
		return 0;
	}

	haveKvaser = InitKvaserDriver();
	if (haveKvaser == false) {
		canUnloadLibrary();
	}

	haveCandleApi = InitCandleDriver();
	if (haveCandleApi == false) {
		// unload
	}

	if ((!haveKvaser) && (!haveCandleApi))
	{
		printf("No CAN adaptor found.\r\n");
		exit(-1);
	}

	// dump can messages
	//while (true) {
	//	if (candle_frame_read(dev, &cframe, 100))
	//	{
	//		printf("ID=%3X(%i): %02X %02X %02X %02X %02X %02X %02X %02X\r\n",
	//			cframe.can_id, cframe.can_dlc, cframe.data[0], cframe.data[1], cframe.data[2], cframe.data[3],
	//			cframe.data[4], cframe.data[5], cframe.data[6], cframe.data[7]);
	//	}
	//}

	if (argc > 1) {
		if (strlen(argv[argc - 1]) > 2) {
			// update firmware
			memset(fwfile, 255, sizeof(fwfile));
			errno_t nErr = fopen_s(&fw, argv[argc - 1], "rb");
			if (nErr == 0) {
				updating = true;
				nFW = (int)fread(fwfile, 1, sizeof(fwfile), fw);
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
	
	if (haveKvaser)
	{
		stat = canBusOn(m_channelData.channel[0].hnd);
	}
	if (haveCandleApi)
	{
		bool hResult = candle_dev_open(dev);
		if (!hResult) exit(-1); // failed
		hResult = candle_channel_set_bitrate(dev, 0, 500000);
		if (!hResult) printf("Could not set bitrate.\r\n");
		hResult = candle_channel_start(dev, 0, CANDLE_MODE_NORMAL);
		if (!hResult) printf("Could not start channel.\r\n");
	}

	if (updating)
		printf("Connected. Press <ESC> to quit. Press F2 to update again.\r\n");
	else
		printf("Connected. Press <ESC> to quit.\r\n");

	if (!updating) {
		char getver[] = "?v\r";
		sendCAN(CAN_MSG_CONSOLE, 3, (unsigned char*)getver);
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
						nFW = (int)fread(fwfile, 1, sizeof(fwfile), fw);
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
				unsigned char ckey = key;
				sendCAN(CAN_MSG_CONSOLE, 1, &ckey);
			}
		}

		while (readCAN()) {
			
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
				sendCAN(CAN_MSG_FIRMWARE, 6, cdata);
				Sleep(2);
				if (updating) {
					// send 64 messages
					for (int ifw = 0; ifw < BLK_FWSIZE; ifw += 8) {
						sendCAN(CAN_MSG_FIRMWARE, 8, &fwfile[iFW + ifw]);
						Sleep(0);
					}
					if (iFW == 0) iFW = -1;
					fwstat = 0;
				}
			}
			else {
				// after timeout, re-transmit firmware packet
			}
		}
		Sleep(0);
	}
	
	if (haveKvaser) {
		for (i = 0; i < m_channelData.channelCount; i++) {
			canStatus stat;
			stat = canBusOff(m_channelData.channel[i].hnd);
			stat = canClose(m_channelData.channel[i].hnd);
		}


		canUnloadLibrary();
	}

	if (haveCandleApi)
	{
		candle_channel_stop(dev, 0);
		candle_dev_close(dev);
	}
	exit(0);
}

