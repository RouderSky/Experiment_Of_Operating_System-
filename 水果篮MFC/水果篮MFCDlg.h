
// ˮ����MFCDlg.h : ͷ�ļ�
//

#pragma once


// Cˮ����MFCDlg �Ի���
class Cˮ����MFCDlg : public CDialogEx
{
// ����
public:
	Cˮ����MFCDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_MFC_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
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
