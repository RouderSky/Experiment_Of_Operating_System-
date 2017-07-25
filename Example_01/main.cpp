#include <windows.h>
#include <iostream>
using namespace std;

DWORD WINAPI FunOne(LPVOID param)
{
	while (*(int*)param)
	{
		cout << "hello!";
		Sleep(1000);    //为什么去掉了Sleep之后，线程挂起之后就没办法再恢复了？
	}
	return 0;
}

DWORD WINAPI FunTwo(LPVOID param)
{
	while (*(int*)param)
	{
		cout << "world!";
		Sleep(1000);	//为什么去掉了Sleep之后，线程挂起之后就没办法再恢复了？
	}
	return 0;
}

int main()
{
	int input = 1;

	//创建线程
	HANDLE hand1 = CreateThread(NULL, 0, FunOne, (void*)&input, CREATE_SUSPENDED, NULL);
	HANDLE hand2 = CreateThread(NULL, 0, FunTwo, (void*)&input, CREATE_SUSPENDED, NULL);

	while (true)
	{
		cout << "main" << endl;
		cin >> input;
		if (input==1)
		{
			ResumeThread(hand1);   //线程启动之后就会分时间片,问题是为什么有时候没有办法回复线程?
			ResumeThread(hand2);   //线程启动之后就会分时间片
		} 
		else
		{
			SuspendThread(hand1);  
			SuspendThread(hand2);
		}
	}

	TerminateThread(hand1, 1);
	TerminateThread(hand2, 1);

	return 0;
}