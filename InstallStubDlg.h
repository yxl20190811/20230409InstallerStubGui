
// InstallStubDlg.h : header file
//

#pragma once
#include "afxwin.h"
#include "afxcmn.h"

#define WM_YY_UPDATE  (WM_USER+100)
// CInstallStubDlg dialog
class CInstallStubDlg : public CDialogEx
{
public:
	static void __cdecl ThreadFun (void *);
	void OnThread();
	HANDLE m_hThread;
	int m_DownloadProgressInt;
	int m_ExactProgressInt;
// Construction
public:
	CInstallStubDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_INSTALLSTUB_DIALOG };

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
	CEdit m_UrlEdit;
	CEdit m_LocalDirEdit;

	CButton m_DownloadButton;
	CString m_DownloadPercent;
	CProgressCtrl m_DownloadProgress;
	
	CString m_EaxctPercent;
	CProgressCtrl m_ExactProgress;
	
	afx_msg void OnClickedDownload();
	CString m_UrlText;
	CString m_LoaclDirText;
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
};
