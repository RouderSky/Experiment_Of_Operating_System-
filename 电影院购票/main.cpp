//--------------------------------------------
/*Copyright (c) 2016 王洪涛 rights reserved.*/
//--------------------------------------------
#include "StdAfx.h"
#include <windows.h>
#include <iostream>
#include <tchar.h>
using namespace std;

//定义座位数
#define NUMOFSEAT 30			//总票数
int a[NUMOFSEAT];				//每个位置对应的票数
CRITICAL_SECTION cs[NUMOFSEAT];	//创建临界区

//线程
DWORD ThreadProc(void * pData)
{
	//电影票编号
	int num = (*(int*)pData);		

	////////////////////////以下是临界区
	//打开对应的互斥量
// 	HANDLE hMutex;
// 	char nameBuffer[100];
// 	memset(nameBuffer, 0, sizeof(nameBuffer));
// 	sprintf_s(nameBuffer, "%d", num);
// 	if ((hMutex = OpenMutex(MUTEX_ALL_ACCESS,FALSE,nameBuffer)) == NULL)		//.....
// 	{
// 		cout << "资源占用失败" << endl;
// 		return 0;
// 	}

	//进入临界区
	EnterCriticalSection(&cs[num]);		

	cout << num << "号座位开始预订" << endl;

	//操作延时
	Sleep(1000);	//在临界区中不会被挂起？

	//检查当前所选位置是否有票
	if (a[num]>0)	//有票
	{
		a[num]--;
		cout << num << "号座位预订完成" << endl;
	}
	else			//无票
	{
		cout << num << "号座位已经被预订" << endl;
	}

	//退出临界区
	LeaveCriticalSection(&cs[num]);

	//操作延时
	Sleep(1000);
	
// 	//释放对应互斥量
// 	ReleaseMutex(hMutex);
// 	////////////////////////
// 
// 	CloseHandle(hMutex);
	return 0;

}

int main()
{
	//初始化票数
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		a[i] = 1;
	}

// 	//创建互斥量
// 	HANDLE hMutex[NUMOFSEAT];
// 	char nameBuf[100];
// 	for (int i = 0; i < NUMOFSEAT;i++)
// 	{
// 		//初始化互斥量数组
// 		memset(nameBuf, 0, sizeof(nameBuf));
// 		sprintf_s(nameBuf, "%d", i);
// 		hMutex[i] = CreateMutex(NULL, FALSE, nameBuf);
// 		if (hMutex[i] == NULL)
// 		{
// 			cout << "互斥量创建失败" << endl;
// 			return 0;
// 		}
// 	}

	//初始化临界区
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		InitializeCriticalSection(&cs[i]);
	}

	//买票操作
	int select;
	while (cin >> select)
	{
		//根据读入的数字去启动线程
		if (select<NUMOFSEAT)
		{
			HANDLE h = CreateThread(NULL,
				0,
				(LPTHREAD_START_ROUTINE)ThreadProc,
				(void *)&select,
				0,
				NULL);

			if (h == NULL)
				cout << "线程创建失败" << endl;
		}
		else
		{
			cout << "没有这个位置" << endl;
		}
	}

// 	//关闭所有互斥量
// 	for (int i = 0; i < NUMOFSEAT;i++)
// 	{
// 		CloseHandle(hMutex[i]);
// 	}

	//释放临界区
	for (int i = 0; i < NUMOFSEAT;i++)
	{
		DeleteCriticalSection(&cs[i]);
	}

	cin.get();
	return 0;
}
