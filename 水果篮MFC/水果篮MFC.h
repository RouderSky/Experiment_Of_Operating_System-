
// ˮ����MFC.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// Cˮ����MFCApp: 
// �йش����ʵ�֣������ ˮ����MFC.cpp
//

class Cˮ����MFCApp : public CWinApp
{
public:
	Cˮ����MFCApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern Cˮ����MFCApp theApp;