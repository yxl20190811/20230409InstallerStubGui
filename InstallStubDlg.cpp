
// InstallStubDlg.cpp : implementation file
//

#include "stdafx.h"
#include "InstallStub.h"
#include "InstallStubDlg.h"
#include "afxdialogex.h"
#include "TExact.h"
#include "TDownload.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CAboutDlg dialog used for App About

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CInstallStubDlg dialog




CInstallStubDlg::CInstallStubDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CInstallStubDlg::IDD, pParent)
	, m_UrlText(_T("https://curl.se/windows/dl-8.0.1_6/curl-8.0.1_6-win64-mingw.zip"))
	, m_LoaclDirText(_T("c:/hh"))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CInstallStubDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_UrlEdit);
	DDX_Control(pDX, IDC_EDIT2, m_LocalDirEdit);

	DDX_Text(pDX, IDC_EDIT3, m_DownloadPercent);
	DDX_Text(pDX, IDC_EDIT4, m_EaxctPercent);
	DDX_Control(pDX, IDC_PROGRESS1, m_DownloadProgress);
	DDX_Control(pDX, IDC_PROGRESS2, m_ExactProgress);
	DDX_Control(pDX, IDC_DOWNLOAD, m_DownloadButton);
	DDX_Text(pDX, IDC_EDIT1, m_UrlText);
	DDX_Text(pDX, IDC_EDIT2, m_LoaclDirText);
}

BEGIN_MESSAGE_MAP(CInstallStubDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_DOWNLOAD, &CInstallStubDlg::OnClickedDownload)
END_MESSAGE_MAP()


// CInstallStubDlg message handlers

BOOL CInstallStubDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CInstallStubDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CInstallStubDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CInstallStubDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CInstallStubDlg::OnClickedDownload()
{
	this->UpdateData(TRUE);
	m_UrlEdit.EnableWindow(FALSE);
	m_LocalDirEdit.EnableWindow(FALSE);
	m_DownloadButton.EnableWindow(FALSE);
	m_DownloadPercent = "";
	m_EaxctPercent = "";
	m_DownloadProgress.SetPos(0);
	m_ExactProgress.SetPos(0);
	m_DownloadProgressInt = 0;
	m_ExactProgressInt = 0;
	this->UpdateData(FALSE);

	m_hThread = (HANDLE)::_beginthread(ThreadFun, 0, this);
}

void __cdecl CInstallStubDlg::ThreadFun (void *pThis){
	((CInstallStubDlg*)pThis)->OnThread();
}
void CInstallStubDlg::OnThread()
{
	// 下载 ZIP 文件到临时目录
	std::string url = m_UrlText.GetBuffer(0);
	std::string extractPath = m_LoaclDirText.GetBuffer(0);
	char zipPath[MAX_PATH];
	tmpnam_s(zipPath, MAX_PATH);
	
	if (downloadFile(url, zipPath, this))
	{
		m_DownloadProgressInt = 100;
		PostMessage(WM_YY_UPDATE, 0, 0);

		// 解压缩 ZIP 文件到指定目录
		if (extractZip(zipPath, extractPath, this)) {
			// 删除临时文件
			remove(zipPath);
		}

	}


	m_UrlEdit.EnableWindow(TRUE);
	m_LocalDirEdit.EnableWindow(TRUE);
	m_DownloadButton.EnableWindow(TRUE);
}


LRESULT CInstallStubDlg::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
	// TODO: 在此添加专用代码和/或调用基类
	if(WM_YY_UPDATE == message){
		m_DownloadProgress.SetPos(m_DownloadProgressInt);
		m_ExactProgress.SetPos(m_ExactProgressInt);
		this->UpdateData(FALSE);
	}
	return CDialogEx::WindowProc(message, wParam, lParam);
}
