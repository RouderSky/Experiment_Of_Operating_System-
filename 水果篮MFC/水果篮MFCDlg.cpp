
// ˮ����MFCDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "ˮ����MFC.h"
#include "ˮ����MFCDlg.h"
#include "afxdialogex.h"


#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//�����ٽ���
CRITICAL_SECTION mutex;		//���ڶ����Ӳ����Ļ��⣬�ô�����
CRITICAL_SECTION mutexForFirstPutApple;
CRITICAL_SECTION mutexForFirstPutOrange;
CRITICAL_SECTION mutexForFirstGetApple;
CRITICAL_SECTION mutexForFirstGetOrange;

//������¼�����������̸߳���
int first_PutApple = 0;
int first_PutOrange = 0;
int first_GetApple = 0;
int first_GetOrange = 0;

//�����ź�����������ֻ��Ҫ�Ժ�������V�����ĺ����оͲ���Openһ��
HANDLE empty;
HANDLE apple;
HANDLE orange;

//����״̬
enum CalathusState{ CNO, CAPPLE, CORANGE };
CalathusState calathusState;

//ˮ��
enum FruitType{ APPLE, ORANGE };

//Ӳ����һ��ָ��
Cˮ����MFCDlg * PForDlg = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// Cˮ����MFCDlg �Ի���



Cˮ����MFCDlg::Cˮ����MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(Cˮ����MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void Cˮ����MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(Cˮ����MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Father, &Cˮ����MFCDlg::OnBnClickedFather)
	ON_BN_CLICKED(Mother, &Cˮ����MFCDlg::OnBnClickedMother)
	ON_BN_CLICKED(Son, &Cˮ����MFCDlg::OnBnClickedSon)
	ON_BN_CLICKED(Daughter, &Cˮ����MFCDlg::OnBnClickedDaughter)
	ON_BN_CLICKED(IDOK, &Cˮ����MFCDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// Cˮ����MFCDlg ��Ϣ�������

BOOL Cˮ����MFCDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO:  �ڴ���Ӷ���ĳ�ʼ������
	//���ӳ�ʼ�ǿյ�
	calathusState = CalathusState::CNO;

	//�����ź���empty 
	if ((empty = CreateSemaphore(NULL, 1, 1, L"empty")) == NULL) {		//Ϊʲô��ʼΪ0�˻��ǿ��ԷŶ���
		cout << "Create empty error!" << endl;
		return 0;
	}

	//�����ź���apple
	if ((apple = CreateSemaphore(NULL, 0, 1, L"apple")) == NULL) {
		cout << "Create apple error!" << endl;
		return 0;
	}

	//�����ź���orange
	if ((orange = CreateSemaphore(NULL, 0, 1, L"orange")) == NULL) {
		cout << "Create orange error!" << endl;
		return 0;
	}

	//��ʼ���ٽ��� 
	InitializeCriticalSection(&mutex);
	InitializeCriticalSection(&mutexForFirstPutApple);
	InitializeCriticalSection(&mutexForFirstPutOrange);
	InitializeCriticalSection(&mutexForFirstGetApple);
	InitializeCriticalSection(&mutexForFirstGetOrange);

	PForDlg = this;	

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

void Cˮ����MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void Cˮ����MFCDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}


	//ͼƬˢ��Ҫ���������ᱻ��ͼ����ˢ��
	ShowPictrue();
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR Cˮ����MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void Cˮ����MFCDlg::OnBnClickedFather()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	FruitType * fruitType = new FruitType();
	*fruitType = FruitType::APPLE;
	_beginthread(PutFruit, 0, fruitType);
}


void Cˮ����MFCDlg::OnBnClickedMother()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	FruitType * fruitType = new FruitType();
	*fruitType = FruitType::ORANGE;
	_beginthread(PutFruit, 0, fruitType);
}


void Cˮ����MFCDlg::OnBnClickedSon()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	_beginthread(GetOrange, 0, NULL);
}


void Cˮ����MFCDlg::OnBnClickedDaughter()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	_beginthread(GetApple, 0, NULL);
	
}

void Cˮ����MFCDlg::PutFruit(LPVOID param)
{
	//����ת��
	FruitType * fruitType = (FruitType*)param;
	FruitType fruit = *fruitType;
	delete(fruitType);

	
	if (fruit==FruitType::APPLE)
	{
		EnterCriticalSection(&mutexForFirstPutApple);
		first_PutApple++;
		if (first_PutApple == 1)
		{
			PForDlg->GetDlgItem(Father)->EnableWindow(FALSE);
		}
		LeaveCriticalSection(&mutexForFirstPutApple);
	}
	else
	{
		EnterCriticalSection(&mutexForFirstPutOrange);
		first_PutOrange++;
		if (first_PutOrange == 1)
		{
			PForDlg->GetDlgItem(Mother)->EnableWindow(FALSE);
		}
		LeaveCriticalSection(&mutexForFirstPutOrange);
	}

	if (calathusState != CalathusState::CNO)
	{
		cout << "������ˮ��!��ȴ�..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"������ˮ������ȴ�...");
	}
	else if (fruit == FruitType::APPLE && first_PutOrange != 0)
	{
		cout << "ĸ�����ڷŽ���!��ȴ�..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"ĸ�����ڷŽ���!��ȴ�...");
	}
	else if (fruit == FruitType::ORANGE && first_PutApple != 0)
	{
		cout << "�������ڷ�ƻ��!��ȴ�..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"�������ڷ�ƻ��!��ȴ�...");
	}

	WaitForSingleObject(empty, INFINITE);

	EnterCriticalSection(&mutex);	//��ʵ��һ����Գ����������������Ƚϴ���Ҫ���ַ�ˮ�������ʱ��Ż��õ�
	if (fruit == FruitType::APPLE)
	{
		cout << "�������ڷ���ƻ����" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"�������ڷ���ƻ����");
		Sleep(1600);

		calathusState = CalathusState::CAPPLE;
		ReleaseSemaphore(apple, 1, NULL);		//V���� 
		ShowPictrue();

		cout << "ƻ���������!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"ƻ���������!");
		Sleep(1600);
	}
	else
	{
		cout << "ĸ�����ڷ��ý���!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"ĸ�����ڷ��ý���!");
		Sleep(1600);

		calathusState = CalathusState::CORANGE;
		ReleaseSemaphore(orange, 1, NULL);		//V���� 
		ShowPictrue();

		cout << "���ӷ������!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"���ӷ������");
		Sleep(1600);
	}
	LeaveCriticalSection(&mutex);

	
	if (fruit == FruitType::APPLE)
	{
		EnterCriticalSection(&mutexForFirstPutApple);
		first_PutApple--;
		if (first_PutApple == 0)
		{
			PForDlg->GetDlgItem(Father)->EnableWindow(true);
		}
		LeaveCriticalSection(&mutexForFirstPutApple);
	}
	else
	{
		EnterCriticalSection(&mutexForFirstPutOrange);
		first_PutOrange--;
		if (first_PutOrange == 0)
		{
			PForDlg->GetDlgItem(Mother)->EnableWindow(true);
		}
		LeaveCriticalSection(&mutexForFirstPutOrange);
	}
	
}

void Cˮ����MFCDlg::GetApple(LPVOID param)
{
	EnterCriticalSection(&mutexForFirstGetApple);
	first_GetApple++;
	if (first_GetApple==1)
	{
		PForDlg->GetDlgItem(Daughter)->EnableWindow(FALSE);
	}
	LeaveCriticalSection(&mutexForFirstGetApple);

	if (calathusState != CalathusState::CAPPLE)
	{
		cout << "����û��ƻ������ȴ�..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"����û��ƻ������ȴ�...");
	}

	WaitForSingleObject(apple, INFINITE);

	EnterCriticalSection(&mutex);	//��ʵ��һ����Գ����������������Ƚϴ���Ҫ���ַ�ˮ�������ʱ��Ż��õ�

	cout << "Ů����ƻ��!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"Ů����ƻ��!");
	Sleep(1600);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);
	ShowPictrue();

	cout << "ƻ��������!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"ƻ��������!");
	Sleep(1600);

	LeaveCriticalSection(&mutex);

	EnterCriticalSection(&mutexForFirstGetApple);
	first_GetApple--;
	if (first_GetApple == 0)
	{
		PForDlg->GetDlgItem(Daughter)->EnableWindow(true);
	}
	LeaveCriticalSection(&mutexForFirstGetApple);
}

void Cˮ����MFCDlg::GetOrange(LPVOID param)
{
	EnterCriticalSection(&mutexForFirstGetOrange);
	first_GetOrange++;
	if (first_GetOrange == 1)
	{
		PForDlg->GetDlgItem(Son)->EnableWindow(FALSE);
	}
	LeaveCriticalSection(&mutexForFirstGetOrange);

	if (calathusState != CalathusState::CORANGE)
	{
		cout << "����û�����ӣ���ȴ�..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"����û�����ӣ���ȴ�...");
	}

	WaitForSingleObject(orange, INFINITE);

	EnterCriticalSection(&mutex);

	cout << "����������!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"����������!");
	Sleep(1600);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);
	ShowPictrue();

	cout << "���ӱ�����!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"���ӱ�����!");
	Sleep(1600);

	LeaveCriticalSection(&mutex);

	EnterCriticalSection(&mutexForFirstGetOrange);
	first_GetOrange--;
	if (first_GetOrange == 0)
	{
		PForDlg->GetDlgItem(Son)->EnableWindow(true);
	}
	LeaveCriticalSection(&mutexForFirstGetOrange);
}

void Cˮ����MFCDlg::ShowPictrue()
{
	int cx, cy;
	CImage  image;
	CRect   rect;

	//����·������ͼƬ  
	if (calathusState == CalathusState::CNO)
	{
		image.Load(_T("res\\Calathus.png"));
	}
	else if (calathusState == CalathusState::CAPPLE)
	{
		image.Load(_T("res\\Apple.png"));
	}
	else
	{
		image.Load(_T("res\\Orange.png"));
	}
		
	//��ȡͼƬ�Ŀ� �߶�  
	cx = image.GetWidth();
	cy = image.GetHeight();

	//��ȡPicture Control�ؼ��Ĵ�С  
	PForDlg->GetDlgItem(ID_Pictrue)->GetWindowRect(&rect);
	//���ͻ���ѡ�е��ؼ���ʾ�ľ���������  
	PForDlg->ScreenToClient(&rect);
	//�����ƶ����ؼ���ʾ������  
	PForDlg->GetDlgItem(ID_Pictrue)->MoveWindow(rect.left, rect.top, cx, cy, TRUE);
	CWnd *pWnd = NULL;
	pWnd = PForDlg->GetDlgItem(ID_Pictrue);//��ȡ�ؼ����  
	pWnd->GetClientRect(&rect);//��ȡ���ָ��ؼ�����Ĵ�С  

	CDC *pDc = NULL;
	pDc = pWnd->GetDC();//��ȡpicture��DC  

	image.Draw(pDc->m_hDC, rect);//��ͼƬ���Ƶ�picture��ʾ��������  

	PForDlg->ReleaseDC(pDc);
}


void Cˮ����MFCDlg::OnBnClickedOk()
{
	// TODO:  �ڴ���ӿؼ�֪ͨ����������
	CDialogEx::OnOK();

	CloseHandle(empty);
	CloseHandle(apple);
	CloseHandle(orange);
}
