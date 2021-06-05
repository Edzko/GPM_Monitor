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
	uint8_t version;
	uint8_t logWiFi : 1;
	uint8_t vlsf : 1;
	uint8_t dreck : 1;  // dead-reckoning model
	uint8_t parseNMEA : 1;
	uint8_t parseUBLOX : 1;
	uint8_t drFlag : 1;
	uint8_t drMakeNovatel : 1;
	uint8_t res1 : 1;
	uint16_t dRate; // debug monitor interval
	uint32_t ubloxbaudrate;
	uint32_t wifibaudrate;
	float GyroCal;  // 16
	float Kf_Accel;
	float Kff_Accel;
	float Kf_Gyro;
	float KG_wo;
	uint16_t gpsRate; // 36
	uint16_t res2;
	double longitude;
	double lattitude;
	float heading;  // 56
	uint16_t year;
	uint8_t month;
	uint8_t day;
	uint8_t hour;
	uint8_t minute;
	int8_t ntrip_un[30]; // 66
	int8_t ntrip_pw[30];
	int8_t ntrip_host[30];
	uint16_t ntrip_port; // 156
	uint16_t dm;   ///< Initial Debug Monitor 
	uint16_t nQ;  ///< Number of epochs data IMU/ADC to capture
	uint8_t ethChan;  ///< COM port used to bridge Ethernet data for Simulink. Follow ::LOGIN_T
	uint8_t drVehcile;
	int16_t Gyro0[3];  ///< Gyro Offsets
	int16_t Acc0[3];  ///< Acceleromater offsets
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
};
