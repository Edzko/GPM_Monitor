// GPM_MonitorDlg.h : header file
//

#pragma once
#include <sys/timeb.h>
#include <time.h>
#include <mmsystem.h> 

#define MAXPNTS 1000

//typedef unsigned long uint64_t;
//typedef unsigned short uint16_t;
//typedef unsigned int uint32_t;
//typedef unsigned char uint8_t;
//typedef char int8_t;
//typedef short int16_t;

typedef union
{
	/** Binary access to GPM data record */
	uint8_t data[100];
	/** Structure data record with GPM information */
	struct {
		uint64_t time;          ///< Current system time
		long double latitude;   ///< Latitude in degrees
		long double longitude;  ///< Longitude in degrees
		float heading;          ///< Current heading
		float std;
		float Vpp;              ///< Power Voltage level in Volts
		uint16_t posType;       ///< GPS positioning Type
		int16_t steer;          ///< Vehicle Steering angle measurement
		int16_t brake;          ///< Vehicle Brake Pressure measurement
		int16_t rpm;            ///< Current engine rpm
		int16_t speed;          ///< Current vehicle speed measurement
		int16_t gear;           ///< Current Vehicle Gear (See JEEP specs for value definition)
		float vehEm_loc; // 44
		float vehNm_loc;
		float vehHrad_loc;
		float error_Em;
		float error_Nm;
		float error_Hdeg; //64
		int16_t nSats;
		uint16_t wheelspeed[4];
		uint16_t WF_State;
		float roll, pitch, yaw; //78
	} rec;
} GPM_T;

typedef struct
{
	uint8_t version;            ///< version of this configuration record.
	uint8_t assistnow : 1;        ///< Use AssistNow at startup
	uint8_t vlsf : 1;             ///< vlsf model active/inactive
	uint8_t dreck : 1;            ///< dead-reckoning model active/inactive
	uint8_t parseNMEA : 1;        ///< option for processing NMEA messages
	uint8_t ntrip_auto : 1;       ///< automatically establish NTRIP connection at startup
	uint8_t drFlag : 1;           ///< Dead-reckoning model parameter value "drFlag"
	uint8_t drMakeNovatel : 1;    ///< Use UBLOX data for Novatel input to Dead-reckoning model
	uint8_t useVLSF : 1;          ///< Use VLSF output instead of raw UBLOX output
	uint16_t dRate;             ///< debug monitor interval in ms
	uint32_t ubloxbaudrate;     ///< internal UART interface baudrate
	uint32_t wifibaudrate;      ///< internal UART interface baudrate
	float GyroCal;              ///< Calibration value for Gyro bias
	float Kf_Accel;             ///< first-order armax filter coefficient for accel
	float Kff_Accel;            ///< second-order armax filter coefficient for accel
	float Kf_Gyro;              ///< first-order armax filter coefficient for gyro
	float KG_wo;                ///< wash-out coefficient for gyro
	uint16_t gpsRate;           ///< message output rate for UBLOX
	uint16_t res2;              ///< unused
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
	int16_t dum;
	float altitude;             ///< GPS altitude
	uint16_t vibpnts;           ///< Number of samples to capture per each FFT
	uint8_t fftWnd;             ///< FFT Filtering Window function for Vibration data
	uint8_t fftsamps;           ///< Number of samples to combine in the fft (downsampling)
	uint8_t fftlog;             ///< Compute fft samples in (1:log) scale (0: linear)
	uint8_t fftplotlines;       ///< Plot lines (1) instead of dots(0) in the display
	float fftscale;             ///< scaling factor for vibration data
	uint16_t fftfreq;            ///< Set frequency for synthesized data
	uint8_t fftaxis;            ///< Select axis to process data for (0 = all)
	uint8_t fftrate;            ///< Sample rate in kHz to capture raw IMU samples
} CONFIG_DATA;


// following must match the embedded firmware
#define MAX_FWSIZE (1500)
#define BLK_FWSIZE (0x200)
#define ERASE_BLOCK_SIZE (16384UL)  // 0x4000

extern unsigned char fwdata[MAX_FWSIZE]; // 1Mb firmware hex file max

// CGPM_MonitorDlg dialog
class CGPM_MonitorDlg : public CDialog
{
// Construction
public:
	CGPM_MonitorDlg(CWnd* pParent = NULL);	// standard constructor
	
// Dialog Data
	enum { IDD = IDD_GPM_MONITOR_DIALOG };
	void GetCOMPorts(CComboBox *pCOM);
	int Connected;
	HANDLE hCommPort;
	COMMTIMEOUTS CommTimeouts;
	unsigned int stat;
	FILE *logFile;
	char logFilename[1000];
	CONFIG_DATA cfgdata;

	CButton *pAutoConnect, *pServer, *pSimulink;
	CComboBox *pCOMList, *pSvrCOMList, *pSimulinkCOMList;
	CSliderCtrl* pRate;
	CProgressCtrl* pUpdate;

	CGPM_AHRS *pAhrsDlg;

	sockaddr_in si_other;
	SOCKET gpmSock;
	bool upgrading;
	bool ambootloader;
	bool svrrunning;
	bool simrunning;
	bool autoselect;
	int svrcom;
	unsigned short crc;
	int iFW, nFW, wFW;
	FILE *fw;

	JOYINFOEX Joystick;
	JOYCAPS JoyCaps;
	MMRESULT mResult;
	UINT JOY_ID;
	int jx, jy, jr, jb;

	int sim_dt;  // in ms
	int sim_cnt; // message count
	int sim_n; // number of bytes in message

	void Send(char *msg, int len);
	void Recv(char *msg, int *len);
	void parseSP(char* msg, int len);
	bool noUpdate;
	
protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support

// Implementation
protected:
	HICON m_hIcon;
	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedConnect();
	afx_msg void OnTimer(UINT_PTR nIDEvent);
	afx_msg void OnBnClickedReset();
	afx_msg void OnDestroy();
	afx_msg void OnBnClickedUpdate();
	afx_msg void OnBnClickedAutoconnect();
	afx_msg void OnBnClickedServer();
	afx_msg void OnBnClickedSvrconnect();
	afx_msg void OnCbnSelchangeSvrcomlist();
	afx_msg void OnBnClickedSimulinkconnect();
	afx_msg void OnBnClickedSimulink();
	afx_msg void OnSelchangeSimulinkcomlist();
	afx_msg void OnCbnSelchangeVehSel();
	afx_msg void OnBnClickeddrFlag();
	afx_msg void OnBnClickedNtrip();
	afx_msg void OnBnClickedHelp();
	afx_msg void OnBnClickedSetutctime();
	afx_msg void OnCbnSelchangeComlist();
	afx_msg void OnBnClickedLoadconfig();
	afx_msg void OnBnClickedSaveconfig();
};
