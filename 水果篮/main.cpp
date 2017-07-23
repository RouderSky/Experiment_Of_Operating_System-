//--------------------------------------------
/*Copyright (c) 2016 ������ rights reserved.*/
//--------------------------------------------
#include "StdAfx.h"
#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//�����ٽ���
CRITICAL_SECTION mutex;		//���ڶ����Ӳ����Ļ��⣬�ô�����
// CRITICAL_SECTION mutexForFirstPutApple;
// CRITICAL_SECTION mutexForFirstPutOrange;
// CRITICAL_SECTION mutexForFirstGetApple;
// CRITICAL_SECTION mutexForFirstGetOrange;

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
enum CalathusState{CNO,CAPPLE,CORANGE};
CalathusState calathusState;

//ˮ��
enum FruitType{APPLE,ORANGE};

//д��һ����������,һ���������߳�д�ɶ�������������������������㣬���������
void PutFruit(LPVOID param)
{
	//����ת��
	FruitType * fruitType = (FruitType*)param;
	FruitType fruit = *fruitType;
	delete(fruitType);

	if (fruit == FruitType::APPLE)
	{
		first_PutApple++;
	}
	else
	{
		first_PutOrange++;
	}

	if (calathusState!=CalathusState::CNO)
	{
		cout << "������ˮ��!��ȴ�..." << endl;
	}
	else if (fruit == FruitType::APPLE && first_PutOrange!=0)
	{
		cout << "ĸ�����ڷŽ���!��ȴ�..." << endl;
	}
	else if (fruit == FruitType::ORANGE && first_PutApple != 0)
	{
		cout << "�������ڷ�ƻ��!��ȴ�..." << endl;
	}

	WaitForSingleObject(empty, INFINITE);

	EnterCriticalSection(&mutex);	//��ʵ��һ����Գ�������������������1��Ҫ���ַ�ˮ�������ʱ��Ż��õ�
	if (fruit == FruitType::APPLE)
	{
		cout << "�������ڷ���ƻ����" << endl;
		Sleep(1000);

		calathusState = CalathusState::CAPPLE;
		ReleaseSemaphore(apple, 1, NULL);		//V���� 

		cout << "ƻ���������!" << endl;
		Sleep(1000);
	}
	else
	{
		cout << "ĸ�����ڷ��ý���!" << endl;
		Sleep(1000);

		calathusState = CalathusState::CORANGE;
		ReleaseSemaphore(orange, 1, NULL);		//V���� 

		cout << "���ӷ������!" << endl;
		Sleep(1000);
	}
	LeaveCriticalSection(&mutex);

	if (fruit == FruitType::APPLE)
	{
		first_PutApple--;
	}
	else
	{
		first_PutOrange--;
	}
}

void GetApple(LPVOID param)
{
	first_GetApple++;

	if (calathusState != CalathusState::CAPPLE)
	{
		cout << "����û��ƻ������ȴ�..." << endl;
	}

	WaitForSingleObject(apple, INFINITE);

	EnterCriticalSection(&mutex);	//��ʵ��һ����Գ����������������Ƚϴ���Ҫ���ַ�ˮ�������ʱ��Ż��õ�

	cout << "Ů����ƻ��!" << endl;
	Sleep(1000);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);

	cout << "ƻ��������!" << endl;
	Sleep(1000);

	LeaveCriticalSection(&mutex);
	first_GetApple--;
}

void GetOrange(LPVOID param)
{
	first_GetOrange++;

	if (calathusState != CalathusState::CORANGE)
	{
		cout << "����û�����ӣ���ȴ�..." << endl;
	}

	WaitForSingleObject(orange, INFINITE);

	EnterCriticalSection(&mutex);

	cout << "����������!" << endl;
	Sleep(1000);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);

	cout << "���ӱ�����!" << endl;
	Sleep(1000);

	LeaveCriticalSection(&mutex);
	first_GetOrange--;
}

int main()
{
	//���ӳ�ʼ�ǿյ�
	calathusState = CalathusState::CNO;

	//�����ź���empty 
	if ((empty = CreateSemaphore(NULL, 1, 1, "empty")) == NULL) {		
		cout << "Create empty error!" << endl;
		return 0;
	}

	//�����ź���apple
	if ((apple = CreateSemaphore(NULL, 0, 1, "apple")) == NULL) {
		cout << "Create apple error!" << endl;
		return 0;
	}

	//�����ź���orange
	if ((orange = CreateSemaphore(NULL, 0, 1, "orange")) == NULL) {
		cout << "Create orange error!" << endl;
		return 0;
	}

	//��ʼ���ٽ��� 
	InitializeCriticalSection(&mutex);
//	InitializeCriticalSection(&mutexForFirstPutApple);
//	InitializeCriticalSection(&mutexForFirstPutOrange);
//	InitializeCriticalSection(&mutexForFirstGetApple);
//	InitializeCriticalSection(&mutexForFirstGetOrange);

	//����
	int operation;
	FruitType * fruitType;
//	HANDLE hTread = NULL;
	while (cin >> operation)
	{
		if (operation==1)		//����
		{
			if (first_PutApple==0)
			{
				fruitType = new FruitType();
				*fruitType = FruitType::APPLE;
				_beginthread(PutFruit, 0, fruitType);
			}
			else
				cout << "���ɼ����������߳�" << endl;
		}
		else if (operation==2)	//ĸ��
		{
			if (first_PutOrange == 0)
			{
				fruitType = new FruitType();
				*fruitType = FruitType::ORANGE;
				_beginthread(PutFruit, 0, fruitType);
			}
			else
				cout << "���ɼ����������߳�" << endl;
		}
		else if (operation==3)	//����
		{
			if (first_GetOrange == 0)
			{
				_beginthread(GetOrange, 0, NULL);
			}
			else
				cout << "���ɼ����������߳�" << endl;
		}
		else if(operation==4)	//Ů��
		{
			if (first_GetApple == 0)
			{
				_beginthread(GetApple, 0, NULL);
			}
			else
				cout << "���ɼ����������߳�" << endl;
		}
//		CloseHandle(hTread);	//����Ҫ���߳̽��ж���Ĳ����Ϳ����ͷ�
	}

	//�ر��ź���
	CloseHandle(empty);
	CloseHandle(apple);
	CloseHandle(orange);

	return 0;
}
