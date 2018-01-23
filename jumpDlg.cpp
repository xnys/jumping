
// jumpDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "jump.h"
#include "jumpDlg.h"
#include "afxdialogex.h"

#include <string>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CjumpDlg 对话框



CjumpDlg::CjumpDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_JUMP_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CjumpDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CjumpDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START, &CjumpDlg::OnBnClickedStart)
	ON_BN_CLICKED(IDCANCEL, &CjumpDlg::OnBnClickedCancel)
	ON_BN_CLICKED(IDC_CHECK, &CjumpDlg::OnBnClickedCheck)
	ON_BN_CLICKED(IDC_PAUSE, &CjumpDlg::OnBnClickedPause)
	ON_WM_DESTROY()
END_MESSAGE_MAP()


// CjumpDlg 消息处理程序

BOOL CjumpDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
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

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	// TODO: 在此添加额外的初始化代码
	m_cmd = ICmdOperator::createCmdOperator();
	m_cmd->init();
	m_buttonType = ButtonType_Check;
	m_isStart = false;
	m_thread = nullptr;
	m_step = 0;
	m_isConnected = false;

	m_cmd->execCmd("md screen\n");

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CjumpDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CjumpDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CjumpDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CjumpDlg::DeleteFolder(CString sPath)
{
	CFileFind ff;
	BOOL bFound;
	bFound = ff.FindFile(sPath + "\\*.*");
	while (bFound)
	{
		bFound = ff.FindNextFile();
		CString sFilePath = ff.GetFilePath();

		if (ff.IsDirectory())
		{
			if (!ff.IsDots())
			{
				DeleteFolder(sFilePath);
			}
		}
		else
		{
			if (ff.IsReadOnly())
			{
				SetFileAttributes(sFilePath, FILE_ATTRIBUTE_NORMAL);
			}
			DeleteFile(sFilePath);
		}
	}
	ff.Close();
	//上面已经把文件夹下面的文件全部删除了，如果需要把文件夹也删除掉则加上以下代码 
	//    SetFileAttributes(sPath, FILE_ATTRIBUTE_NORMAL);  //设置文件夹的属性 
	//    RemoveDirectory(sPath);  //删除文件夹 
}

#include <time.h>
int CjumpDlg::run()
{
	DeleteFolder("screen");

	while (m_isStart)
	{
		++m_step;

		//截屏
		m_cmd->execCmd("adb shell screencap -p /sdcard/autojump.png\n");
		//获取到PC
		char command[128] = { 0 };
		sprintf_s(command, "adb pull /sdcard/autojump.png screen/autojump%d.png\n", m_step);
		m_cmd->execCmd(command);
		{
			// 加载PC截取到的图像
			char path[128] = { 0 };
			sprintf_s(path, "screen/autojump%d.png", m_step);
			if (m_findPoint.setPath(path))
			{
				CRect rect;
				CWnd *pWnd = NULL;
				pWnd = GetDlgItem(IDC_PICTURE);//获取控件句柄  
				pWnd->GetClientRect(&rect);//获取句柄指向控件区域的大小  
				CDC *pDc = NULL;
				pDc = pWnd->GetDC();//获取picture的DC  
				m_findPoint.getImg().Draw(pDc->m_hDC, rect);//将图片绘制到picture表示的区域内

				if (m_findPoint.calculaate())
				{
					int sx = m_findPoint.getSX();
					int sy = m_findPoint.getSY();
					int dx = m_findPoint.getDX();
					int dy = m_findPoint.getDY();
					int distance = sqrt(pow((sx - dx), 2) + pow((sy - dy), 2));
					double ratio = min(1.395, 1.55 - 0.0003 * distance);
					int ms = max(distance * ratio, 80);
					TRACE("第%03d步:%d\n", m_step, ms);

					pDc->SetBkMode(TRANSPARENT);
					char pos[22] = { 0 };
					sprintf_s(pos, "[S]: (%4d, %4d)", sx, sy);
					pDc->TextOut(10, 10, pos);
					sprintf_s(pos, "[D]: (%4d, %4d)", dx, dy);
					pDc->TextOut(10, 30, pos);
					sprintf_s(pos, "[T]:  %4d ms", ms);
					pDc->TextOut(10, 50, pos);

					double wratio = rect.Width() * 1.0 / m_findPoint.getWidth(); // 216.0
					double hratio = rect.Height() * 1.0 / m_findPoint.getHeight(); // 383.0
					int tsx = int(sx * wratio);
					int tsy = int(sy * hratio);
					int tdx = int(dx * wratio);
					int tdy = int(dy * hratio);
					if (tsy > 10 && tdy > 10)
					{
						CBrush brush, *oldbrush;
						brush.CreateSolidBrush(RGB(250, 0, 0));
						oldbrush = pDc->SelectObject(&brush);
						pDc->Ellipse(tsx - 4, tsy - 4, tsx + 4, tsy + 4);
						pDc->Ellipse(tdx - 4, tdy - 4, tdx + 4, tdy + 4);
						pDc->SelectObject(oldbrush);
					}
					ReleaseDC(pDc);

					//模拟按键，加入随机数，防止系统识别为机器作弊
					int pressx = sx + rand() % 100 - 50;
					int pressy = tsy + 100 + rand() % 200;
					std::string command = "adb shell input swipe";
					command += " ";
					command += std::to_string(pressx);
					command += " ";
					command += std::to_string(pressy);
					command += " ";
					command += std::to_string(pressx + rand() % 20 - 10);
					command += " ";
					command += std::to_string(pressy - rand() % 20);
					command += " ";
					command += std::to_string(ms);
					command += " \n";
					m_cmd->execCmd(command.c_str());

				}
				else
				{
					goto AutoExit;
				}

				ReleaseDC(pDc);
				Sleep(1700 + rand() % 300 + max(rand() % 1000 - 800, 0));
				continue;
			}
			goto AutoExit;
		}
	}

	if (false)
	{
	AutoExit:
		m_isStart = false;
		CButton*pButton = (CButton*)GetDlgItem(IDC_START);
		pButton->EnableWindow(TRUE);
	}

	return 0;
}

void CjumpDlg::stop()
{
	m_isStart = false;
	if (nullptr != m_thread)
	{
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
}

void CjumpDlg::OnBnClickedStart()
{
	CButton *pButton = (CButton*)GetDlgItem(IDC_START);
	pButton->EnableWindow(FALSE);
	m_isStart = true;
	m_step = 0;
	m_height = 0;
	m_useheight = 5;
	if (nullptr != m_thread)
	{
		m_thread->join();
		delete m_thread;
		m_thread = nullptr;
	}
	m_thread = new std::thread(std::bind(&CjumpDlg::run, this));
}

void CjumpDlg::OnBnClickedCancel()
{
	stop();

	CDialogEx::OnCancel();
}

void CjumpDlg::OnBnClickedCheck()
{
	char result[1024] = { 0 };
	m_cmd->execCmd("adb devices\n", result, 1024);

	std::string checkDevices = result;
	int index = checkDevices.find("devices");
	if (index > 0)
	{
		int pos = checkDevices.find("device", index + 1);
		if (pos > index)
		{
			CButton *pButton = (CButton*)GetDlgItem(IDC_CHECK);
			pButton->EnableWindow(FALSE);
		}
	}
}

void CjumpDlg::OnBnClickedPause()
{
	stop();
	CButton*pButton = (CButton*)GetDlgItem(IDC_START);
	pButton->EnableWindow(TRUE);
}


void CjumpDlg::OnDestroy()
{
	m_cmd->close();
	ICmdOperator::destroy(m_cmd);
	CDialogEx::OnDestroy();
}
