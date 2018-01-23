
// jumpDlg.h : 头文件
//

#pragma once

#include <atomic>
#include <thread>
#include <tuple>
#include "ICmdOperator.h"
#include "CFindPoint.h"

// CjumpDlg 对话框
class CjumpDlg : public CDialogEx
{
// 构造
public:
	CjumpDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_JUMP_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	void DeleteFolder(CString sPath);
	HICON m_hIcon;
	ICmdOperator* m_cmd;

	enum EmButtonType
	{
		ButtonType_Check = 1,
		ButtonType_Start,
	};
	int                    m_buttonType;

	int  run();
	void stop();

// 	std::tuple<bool, int> oneStep();
	std::atomic<bool>      m_isStart;
	std::thread*           m_thread;
	int                    m_step;
	bool                   m_isConnected;
	int                    m_height; // 记录柱子高度
	int                    m_useheight; // 记录柱子高度


	CFindPoint        m_findPoint;
	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStart();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnBnClickedCheck();
	afx_msg void OnBnClickedPause();
	afx_msg void OnDestroy();
};
