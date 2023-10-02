// GPM_USBinterface.h : main header file for the GPM_USBinterface DLL
//

#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"		// main symbols


// CGPMUSBinterfaceApp
// See GPM_USBinterface.cpp for the implementation of this class
//

typedef struct
{
    uint8_t version;            ///< version of this configuration record.
    uint8_t assistnow:1;        ///< Use AssistNow at startup
    uint8_t dummy:1;
    uint8_t gpscan:1;           ///< CAN port to send GPS data to
    uint8_t parseNMEA:1;        ///< option for processing NMEA messages
    uint8_t ntrip_auto:1;       ///< automatically establish NTRIP connection at startup
    uint8_t drFlag:1;           ///< Dead-reckoning model parameter value "drFlag"
    uint8_t drMakeNovatel:1;    ///< Use UBLOX data for Novatel input to Dead-reckoning model
    uint8_t useVLSF:1;          ///< Use VLSF output instead of raw UBLOX output
    uint16_t dRate;             ///< debug monitor interval in ms
    uint32_t ubloxbaudrate;     ///< internal UART interface baudrate
    uint32_t wifibaudrate;      ///< internal UART interface baudrate
    float GyroCal;              ///< Calibration value for Gyro bias
    float Kf_Accel;             ///< first-order armax filter coefficient for accel
    float Kff_Accel;            ///< second-order armax filter coefficient for accel
    float Kf_Gyro;              ///< first-order armax filter coefficient for gyro
    float KG_wo;                ///< wash-out coefficient for gyro
    uint16_t gpsRate;           ///< message output rate for UBLOX
    uint8_t App;                ///< Active Application
    int8_t timezone;           ///< Timezone offset relative to UTC
    double longitude;           ///< recent longitude
    double lattitude;           ///< recent lattitude
    float heading;              ///< recent heading
    uint16_t year;
    uint8_t month;
    uint8_t day;
    uint8_t hour;
    uint8_t minute;
    int8_t ntrip_un[30];        ///< Username for NTRIP login
    int8_t ntrip_pw[30];        ///< Password for NTRIP login
    int8_t ntrip_host[30];      ///< Hostname (ip-address) for NTRIP server
    int8_t ntrip_stream[30];    ///< Stream name for NTRIP server
    uint16_t ntrip_port;        ///< Port number for NTRIP 
    uint16_t dm;                ///< Initial Debug Monitor 
    uint16_t nQ;                ///< Number of epochs data IMU/ADC to capture
    uint8_t ethChan;            ///< COM port used to bridge Ethernet data for Simulink. Follow ::LOGIN_T
    uint8_t drVehcile;          ///< Vehicle ID number for Simulink Dead-reckoning model
    int16_t Gyro0[3];           ///< Gyro Offsets
    int16_t Acc0[3];            ///< Acceleromater offsets
    int16_t rtcctrim;           ///< RTCC trim value (see documentation)
    int16_t rtccadj;            ///< RTCC adjustment (as in 1 second per <adj> seconds)
    int16_t ant_xoff;           ///< Antenna offset in vehcile x direction (vehicle fwd)
    int16_t ant_yoff;           ///< Antenna offset in vehcile y direction (vehicle left)
    int16_t ant_zoff;           ///< Antenna offset in vehcile z direction (vehicle left)
    int16_t canbase;            ///< Base ID for GPS messages on CAN bus
    float altitude;             ///< GPS altitude
    uint16_t vibpnts;           ///< Number of samples to capture per each FFT
    uint8_t fftWnd;            ///< FFT Filtering Window function for Vibration data
    uint8_t fftsamps;           ///< Number of samples to combine in the fft (downsampling)
    uint8_t fftlog;             ///< Compute fft samples in (1:log) scale (0: linear)
    uint8_t fftplotlines;       ///< Plot lines (1) instead of dots(0) in the display
    float fftscale;             ///< scaling factor for vibration data
    uint16_t fftfreq;            ///< Set frequency for synthesized data
    uint8_t fftaxis;            ///< Select axis to process data for (0 = all)
    uint8_t fftrate;            ///< Sample rate in kHz to capture raw IMU samples
    int8_t pm_name[20];             ///< Name of the device for acquisition
    uint32_t wftimeout;         ///< Timeout in milliseconds, when waiting for a connection to a server on the network.
    int8_t ntp_host[30];        ///< hostname of the NTP DayTime server to use
    uint16_t ntp_port;          ///< port number of the NTP DayTime server to use
    int8_t mqtt_host[30];        ///< hostname of the MQTT server to use
    uint16_t mqtt_port;          ///< port number of the MQTT server to use
    uint16_t mqtt_interval;     ///< Update interval 
    uint8_t vib_auto;            ///< automatically start sending vibration data to cloud
    uint8_t mqtt_topic[100];     ///< Main Topic to submit data under
    uint8_t wifi_name[20];      ///< SSID name of the WiFi network
    uint8_t wifi_password[20];  ///< Password for the WiFi
    uint8_t wifi_update;        ///< Flag to indicate that the name and/or password have changed and should be updated on the WiFi module
    uint32_t canfilter[40];     ///< CAN filters. '0' terminates list. Empty list means no filtering.
} CONFIG_DATA;

class CGPMUSBinterfaceApp : public CWinApp
{
public:

       
    CONFIG_DATA cfgdata;

	CGPMUSBinterfaceApp();


// Overrides
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};
