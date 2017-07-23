#include <windows.h>
#include <iostream>
using namespace std;

DWORD WINAPI FunOne(LPVOID param)
{
	while (*(int*)param)
	{
		cout << "hello!";
		Sleep(1000);    //Ϊʲôȥ����Sleep֮���̹߳���֮���û�취�ٻָ��ˣ�
	}
	return 0;
}

DWORD WINAPI FunTwo(LPVOID param)
{
	while (*(int*)param)
	{
		cout << "world!";
		Sleep(1000);	//Ϊʲôȥ����Sleep֮���̹߳���֮���û�취�ٻָ��ˣ�
	}
	return 0;
}

int main()
{
	int input = 1;

	//�����߳�
	HANDLE hand1 = CreateThread(NULL, 0, FunOne, (void*)&input, CREATE_SUSPENDED, NULL);
	HANDLE hand2 = CreateThread(NULL, 0, FunTwo, (void*)&input, CREATE_SUSPENDED, NULL);

	while (true)
	{
		cout << "main" << endl;
		cin >> input;
		if (input==1)
		{
			ResumeThread(hand1);   //�߳�����֮��ͻ��ʱ��Ƭ,������Ϊʲô��ʱ��û�а취�ظ��߳�?
			ResumeThread(hand2);   //�߳�����֮��ͻ��ʱ��Ƭ
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