
// 水果篮MFCDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "水果篮MFC.h"
#include "水果篮MFCDlg.h"
#include "afxdialogex.h"


#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//声明临界区
CRITICAL_SECTION mutex;		//用于对篮子操作的互斥，用处不大
CRITICAL_SECTION mutexForFirstPutApple;
CRITICAL_SECTION mutexForFirstPutOrange;
CRITICAL_SECTION mutexForFirstGetApple;
CRITICAL_SECTION mutexForFirstGetOrange;

//用来记录创建出来的线程个数
int first_PutApple = 0;
int first_PutOrange = 0;
int first_GetApple = 0;
int first_GetOrange = 0;

//声明信号量，这样在只需要对函数进行V操作的函数中就不用Open一次
HANDLE empty;
HANDLE apple;
HANDLE orange;

//篮子状态
enum CalathusState{ CNO, CAPPLE, CORANGE };
CalathusState calathusState;

//水果
enum FruitType{ APPLE, ORANGE };

//硬加上一个指针
C水果篮MFCDlg * PForDlg = NULL;

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
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


// C水果篮MFCDlg 对话框



C水果篮MFCDlg::C水果篮MFCDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(C水果篮MFCDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void C水果篮MFCDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(C水果篮MFCDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(Father, &C水果篮MFCDlg::OnBnClickedFather)
	ON_BN_CLICKED(Mother, &C水果篮MFCDlg::OnBnClickedMother)
	ON_BN_CLICKED(Son, &C水果篮MFCDlg::OnBnClickedSon)
	ON_BN_CLICKED(Daughter, &C水果篮MFCDlg::OnBnClickedDaughter)
	ON_BN_CLICKED(IDOK, &C水果篮MFCDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// C水果篮MFCDlg 消息处理程序

BOOL C水果篮MFCDlg::OnInitDialog()
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

	// TODO:  在此添加额外的初始化代码
	//篮子初始是空的
	calathusState = CalathusState::CNO;

	//创建信号量empty 
	if ((empty = CreateSemaphore(NULL, 1, 1, L"empty")) == NULL) {		//为什么初始为0了还是可以放东西
		cout << "Create empty error!" << endl;
		return 0;
	}

	//创建信号量apple
	if ((apple = CreateSemaphore(NULL, 0, 1, L"apple")) == NULL) {
		cout << "Create apple error!" << endl;
		return 0;
	}

	//创建信号量orange
	if ((orange = CreateSemaphore(NULL, 0, 1, L"orange")) == NULL) {
		cout << "Create orange error!" << endl;
		return 0;
	}

	//初始化临界区 
	InitializeCriticalSection(&mutex);
	InitializeCriticalSection(&mutexForFirstPutApple);
	InitializeCriticalSection(&mutexForFirstPutOrange);
	InitializeCriticalSection(&mutexForFirstGetApple);
	InitializeCriticalSection(&mutexForFirstGetOrange);

	PForDlg = this;	

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void C水果篮MFCDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void C水果篮MFCDlg::OnPaint()
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


	//图片刷新要放这里，否则会被绘图函数刷掉
	ShowPictrue();
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR C水果篮MFCDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void C水果篮MFCDlg::OnBnClickedFather()
{
	// TODO:  在此添加控件通知处理程序代码
	FruitType * fruitType = new FruitType();
	*fruitType = FruitType::APPLE;
	_beginthread(PutFruit, 0, fruitType);
}


void C水果篮MFCDlg::OnBnClickedMother()
{
	// TODO:  在此添加控件通知处理程序代码
	FruitType * fruitType = new FruitType();
	*fruitType = FruitType::ORANGE;
	_beginthread(PutFruit, 0, fruitType);
}


void C水果篮MFCDlg::OnBnClickedSon()
{
	// TODO:  在此添加控件通知处理程序代码
	_beginthread(GetOrange, 0, NULL);
}


void C水果篮MFCDlg::OnBnClickedDaughter()
{
	// TODO:  在此添加控件通知处理程序代码
	_beginthread(GetApple, 0, NULL);
	
}

void C水果篮MFCDlg::PutFruit(LPVOID param)
{
	//立刻转换
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
		cout << "篮子有水果!请等待..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"篮子有水果，请等待...");
	}
	else if (fruit == FruitType::APPLE && first_PutOrange != 0)
	{
		cout << "母亲已在放桔子!请等待..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"母亲已在放桔子!请等待...");
	}
	else if (fruit == FruitType::ORANGE && first_PutApple != 0)
	{
		cout << "父亲已在放苹果!请等待..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"父亲已在放苹果!请等待...");
	}

	WaitForSingleObject(empty, INFINITE);

	EnterCriticalSection(&mutex);	//其实这一句可以出掉，当篮子容量比较大且要保持放水果互斥的时候才会用到
	if (fruit == FruitType::APPLE)
	{
		cout << "父亲正在放置苹果！" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"父亲正在放置苹果！");
		Sleep(1600);

		calathusState = CalathusState::CAPPLE;
		ReleaseSemaphore(apple, 1, NULL);		//V操作 
		ShowPictrue();

		cout << "苹果放置完毕!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"苹果放置完毕!");
		Sleep(1600);
	}
	else
	{
		cout << "母亲正在放置桔子!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"母亲正在放置桔子!");
		Sleep(1600);

		calathusState = CalathusState::CORANGE;
		ReleaseSemaphore(orange, 1, NULL);		//V操作 
		ShowPictrue();

		cout << "桔子放置完毕!" << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"桔子放置完毕");
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

void C水果篮MFCDlg::GetApple(LPVOID param)
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
		cout << "篮子没有苹果，请等待..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"篮子没有苹果，请等待...");
	}

	WaitForSingleObject(apple, INFINITE);

	EnterCriticalSection(&mutex);	//其实这一句可以出掉，当篮子容量比较大且要保持放水果互斥的时候才会用到

	cout << "女儿拿苹果!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"女儿拿苹果!");
	Sleep(1600);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);
	ShowPictrue();

	cout << "苹果被拿走!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"苹果被拿走!");
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

void C水果篮MFCDlg::GetOrange(LPVOID param)
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
		cout << "篮子没有橘子，请等待..." << endl;
		PForDlg->GetDlgItem(static_text)->SetWindowText(L"篮子没有橘子，请等待...");
	}

	WaitForSingleObject(orange, INFINITE);

	EnterCriticalSection(&mutex);

	cout << "儿子拿橘子!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"儿子拿橘子!");
	Sleep(1600);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);
	ShowPictrue();

	cout << "橘子被拿走!" << endl;
	PForDlg->GetDlgItem(static_text)->SetWindowText(L"橘子被拿走!");
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

void C水果篮MFCDlg::ShowPictrue()
{
	int cx, cy;
	CImage  image;
	CRect   rect;

	//根据路径载入图片  
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
		
	//获取图片的宽 高度  
	cx = image.GetWidth();
	cy = image.GetHeight();

	//获取Picture Control控件的大小  
	PForDlg->GetDlgItem(ID_Pictrue)->GetWindowRect(&rect);
	//将客户区选中到控件表示的矩形区域内  
	PForDlg->ScreenToClient(&rect);
	//窗口移动到控件表示的区域  
	PForDlg->GetDlgItem(ID_Pictrue)->MoveWindow(rect.left, rect.top, cx, cy, TRUE);
	CWnd *pWnd = NULL;
	pWnd = PForDlg->GetDlgItem(ID_Pictrue);//获取控件句柄  
	pWnd->GetClientRect(&rect);//获取句柄指向控件区域的大小  

	CDC *pDc = NULL;
	pDc = pWnd->GetDC();//获取picture的DC  

	image.Draw(pDc->m_hDC, rect);//将图片绘制到picture表示的区域内  

	PForDlg->ReleaseDC(pDc);
}


void C水果篮MFCDlg::OnBnClickedOk()
{
	// TODO:  在此添加控件通知处理程序代码
	CDialogEx::OnOK();

	CloseHandle(empty);
	CloseHandle(apple);
	CloseHandle(orange);
}
