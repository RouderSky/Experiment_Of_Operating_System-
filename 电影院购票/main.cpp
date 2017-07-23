//--------------------------------------------
/*Copyright (c) 2016 ������ rights reserved.*/
//--------------------------------------------
#include "StdAfx.h"
#include <windows.h>
#include <iostream>
#include <tchar.h>
using namespace std;

//������λ��
#define NUMOFSEAT 30			//��Ʊ��
int a[NUMOFSEAT];				//ÿ��λ�ö�Ӧ��Ʊ��
CRITICAL_SECTION cs[NUMOFSEAT];	//�����ٽ���

//�߳�
DWORD ThreadProc(void * pData)
{
	//��ӰƱ���
	int num = (*(int*)pData);		

	////////////////////////�������ٽ���
	//�򿪶�Ӧ�Ļ�����
// 	HANDLE hMutex;
// 	char nameBuffer[100];
// 	memset(nameBuffer, 0, sizeof(nameBuffer));
// 	sprintf_s(nameBuffer, "%d", num);
// 	if ((hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,nameBuffer)) == NULL)		//.....
// 	{
// 		cout << "��Դռ��ʧ��" << endl;
// 		return 0;
// 	}

	//�����ٽ���
	EnterCriticalSection(&cs[num]);		

	cout << num << "����λ��ʼԤ��" << endl;

	//������ʱ
	Sleep(1000);	//���ٽ����в��ᱻ����

	//��鵱ǰ��ѡλ���Ƿ���Ʊ
	if (a[num]>0)	//��Ʊ
	{
		a[num]--;
		cout << num << "����λԤ�����" << endl;
	}
	else			//��Ʊ
	{
		cout << num << "����λ�Ѿ���Ԥ��" << endl;
	}

	//�˳��ٽ���
	LeaveCriticalSection(&cs[num]);

	//������ʱ
	Sleep(1000);
	
// 	//�ͷŶ�Ӧ������
// 	ReleaseMutex(hMutex);
// 	////////////////////////
// 
// 	CloseHandle(hMutex);
	return 0;

}

int main()
{
	//��ʼ��Ʊ��
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		a[i] = 1;
	}

// 	//����������
// 	HANDLE hMutex[NUMOFSEAT];
// 	char nameBuf[100];
// 	for (int i = 0; i < NUMOFSEAT;i++)
// 	{
// 		//��ʼ������������
// 		memset(nameBuf, 0, sizeof(nameBuf));
// 		sprintf_s(nameBuf, "%d", i);
// 		hMutex[i] = CreateMutex(NULL, FALSE, nameBuf);
// 		if (hMutex[i] == NULL)
// 		{
// 			cout << "����������ʧ��" << endl;
// 			return 0;
// 		}
// 	}

	//��ʼ���ٽ���
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		InitializeCriticalSection(&cs[i]);
	}

	//��Ʊ����
	int select;
	while (cin >> select)
	{
		//���ݶ��������ȥ�����߳�
		if (select<NUMOFSEAT)
		{
			HANDLE h = CreateThread(NULL,
				0,
				(LPTHREAD_START_ROUTINE)ThreadProc,
				(void *)&select,
				0,
				NULL);

			if (h == NULL)
				cout << "�̴߳���ʧ��" << endl;
		}
		else
		{
			cout << "û�����λ��" << endl;
		}
	}

// 	//�ر����л�����
// 	for (int i = 0; i < NUMOFSEAT;i++)
// 	{
// 		CloseHandle(hMutex[i]);
// 	}

	//�ͷ��ٽ���
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		DeleteCriticalSection(&cs[i]);
	}

	cin.get();
	return 0;
}
