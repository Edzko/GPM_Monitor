// GPM_MonitorDlg.h : header file
//

#pragma once
#include <sys/timeb.h>
#include <time.h>
#define MAXPNTS 1000

typedef unsigned long uint64_t;
typedef unsigned short uint16_t;
typedef short int16_t;

typedef union
{
	unsigned char data[50];
	struct {
		uint64_t time;
		double latitude;
		double longitude;
		float heading;
		float std;
		float Vpp;
		uint16_t posType;
		int16_t steer;
		int16_t brake;
		int16_t rpm;
		int16_t speed;
		int16_t gear;
	} rec;
} GPM_T;
#define MAX_FWSIZE (2048)
#define BLK_FWSIZE (256)

extern char fwdata[MAX_FWSIZE]; // 1Mb firmware hex file max

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
	char logFilename[100];

	CButton *pAutoConnect, *pServer, *pSimulink;
	CComboBox *pCOMList, *pSvrCOMList, *pSimulinkCOMList;

	sockaddr_in si_other;
	SOCKET gpmSock;
	bool upgrading;
	bool ambootloader;
	bool svrrunning;
	bool simrunning;
	int svrcom;
	unsigned int crc;
	int iFW, nFW, wFW;
	FILE *fw;

	void Send(char *msg, int len);
	void Recv(char *msg, int *len);
	
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
};
