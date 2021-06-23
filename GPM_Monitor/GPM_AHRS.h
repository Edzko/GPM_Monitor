#pragma once


// CGPM_AHRS dialog

class CGPM_AHRS : public CDialog
{
	DECLARE_DYNAMIC(CGPM_AHRS)

public:
	CGPM_AHRS(CWnd* pParent = nullptr);   // standard constructor
	virtual ~CGPM_AHRS();

	bool myUpdate, firstUpdate;
	void DrawCompass(CDC* dc, int CID);
	void DrawOrientation(CDC* dc, int CID);
	void Update();
	void UpdateFilter(void);
	void ResizeAhrs(RECT* rc);
	double roll, pitch, yaw, roll1, pitch1, yaw1, roll2, pitch2, yaw2;
	double _time, _vpp, Kf;
	CFont tdFont;
	CBrush bg;
	CWnd* pDb;
	CDC* dc;
	LPCSTR MyCursor;

// Dialog Data
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_AHRS };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
	afx_msg void OnPaint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
};
