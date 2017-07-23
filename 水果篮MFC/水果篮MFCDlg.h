
// 水果篮MFCDlg.h : 头文件
//

#pragma once


// C水果篮MFCDlg 对话框
class C水果篮MFCDlg : public CDialogEx
{
// 构造
public:
	C水果篮MFCDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
	enum { IDD = IDD_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedFather();
	afx_msg void OnBnClickedMother();
	afx_msg void OnBnClickedSon();
	afx_msg void OnBnClickedDaughter();

	static void PutFruit(LPVOID param);
	static void GetApple(LPVOID param);
	static void GetOrange(LPVOID param);
	static void ShowPictrue();
	afx_msg void OnBnClickedOk();
};
