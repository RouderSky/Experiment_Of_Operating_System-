//--------------------------------------------
/*Copyright (c) 2016 王洪涛 rights reserved.*/
//--------------------------------------------
#include "StdAfx.h"
#include <windows.h>
#include <process.h>
#include <iostream>
using namespace std;

//声明临界区
CRITICAL_SECTION mutex;		//用于对篮子操作的互斥，用处不大
// CRITICAL_SECTION mutexForFirstPutApple;
// CRITICAL_SECTION mutexForFirstPutOrange;
// CRITICAL_SECTION mutexForFirstGetApple;
// CRITICAL_SECTION mutexForFirstGetOrange;

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
enum CalathusState{CNO,CAPPLE,CORANGE};
CalathusState calathusState;

//水果
enum FruitType{APPLE,ORANGE};

//写成一个函数不好,一个尽量将线程写成多个函数，这样控制起来跟方便，代码更精炼
void PutFruit(LPVOID param)
{
	//立刻转换
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
		cout << "篮子有水果!请等待..." << endl;
	}
	else if (fruit == FruitType::APPLE && first_PutOrange!=0)
	{
		cout << "母亲已在放桔子!请等待..." << endl;
	}
	else if (fruit == FruitType::ORANGE && first_PutApple != 0)
	{
		cout << "父亲已在放苹果!请等待..." << endl;
	}

	WaitForSingleObject(empty, INFINITE);

	EnterCriticalSection(&mutex);	//其实这一句可以出掉，当篮子容量大于1且要保持放水果互斥的时候才会用到
	if (fruit == FruitType::APPLE)
	{
		cout << "父亲正在放置苹果！" << endl;
		Sleep(1000);

		calathusState = CalathusState::CAPPLE;
		ReleaseSemaphore(apple, 1, NULL);		//V操作 

		cout << "苹果放置完毕!" << endl;
		Sleep(1000);
	}
	else
	{
		cout << "母亲正在放置桔子!" << endl;
		Sleep(1000);

		calathusState = CalathusState::CORANGE;
		ReleaseSemaphore(orange, 1, NULL);		//V操作 

		cout << "桔子放置完毕!" << endl;
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
		cout << "篮子没有苹果，请等待..." << endl;
	}

	WaitForSingleObject(apple, INFINITE);

	EnterCriticalSection(&mutex);	//其实这一句可以出掉，当篮子容量比较大且要保持放水果互斥的时候才会用到

	cout << "女儿拿苹果!" << endl;
	Sleep(1000);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);

	cout << "苹果被拿走!" << endl;
	Sleep(1000);

	LeaveCriticalSection(&mutex);
	first_GetApple--;
}

void GetOrange(LPVOID param)
{
	first_GetOrange++;

	if (calathusState != CalathusState::CORANGE)
	{
		cout << "篮子没有橘子，请等待..." << endl;
	}

	WaitForSingleObject(orange, INFINITE);

	EnterCriticalSection(&mutex);

	cout << "儿子拿橘子!" << endl;
	Sleep(1000);

	calathusState = CalathusState::CNO;
	ReleaseSemaphore(empty, 1, NULL);

	cout << "橘子被拿走!" << endl;
	Sleep(1000);

	LeaveCriticalSection(&mutex);
	first_GetOrange--;
}

int main()
{
	//篮子初始是空的
	calathusState = CalathusState::CNO;

	//创建信号量empty 
	if ((empty = CreateSemaphore(NULL, 1, 1, "empty")) == NULL) {		
		cout << "Create empty error!" << endl;
		return 0;
	}

	//创建信号量apple
	if ((apple = CreateSemaphore(NULL, 0, 1, "apple")) == NULL) {
		cout << "Create apple error!" << endl;
		return 0;
	}

	//创建信号量orange
	if ((orange = CreateSemaphore(NULL, 0, 1, "orange")) == NULL) {
		cout << "Create orange error!" << endl;
		return 0;
	}

	//初始化临界区 
	InitializeCriticalSection(&mutex);
//	InitializeCriticalSection(&mutexForFirstPutApple);
//	InitializeCriticalSection(&mutexForFirstPutOrange);
//	InitializeCriticalSection(&mutexForFirstGetApple);
//	InitializeCriticalSection(&mutexForFirstGetOrange);

	cout << "输入数字1~4并回车即可发出命令：\n" << "1代表父亲放苹果\n" << "2代表母亲放橘子\n" << "3代表儿子拿橘子\n" << "4代表女儿拿苹果\n" << "ps：篮子只有一个位置\n";

	//操作
	int operation;
	FruitType * fruitType;
//	HANDLE hTread = NULL;
	while (cin >> operation)
	{
		if (operation==1)		//父亲
		{
			if (first_PutApple==0)
			{
				fruitType = new FruitType();
				*fruitType = FruitType::APPLE;
				_beginthread(PutFruit, 0, fruitType);
			}
			else
				cout << "不可继续创建该线程" << endl;
		}
		else if (operation==2)	//母亲
		{
			if (first_PutOrange == 0)
			{
				fruitType = new FruitType();
				*fruitType = FruitType::ORANGE;
				_beginthread(PutFruit, 0, fruitType);
			}
			else
				cout << "不可继续创建该线程" << endl;
		}
		else if (operation==3)	//儿子
		{
			if (first_GetOrange == 0)
			{
				_beginthread(GetOrange, 0, NULL);
			}
			else
				cout << "不可继续创建该线程" << endl;
		}
		else if(operation==4)	//女儿
		{
			if (first_GetApple == 0)
			{
				_beginthread(GetApple, 0, NULL);
			}
			else
				cout << "不可继续创建该线程" << endl;
		}
//		CloseHandle(hTread);	//不需要对线程进行额外的操作就可以释放
	}

	//关闭信号量
	CloseHandle(empty);
	CloseHandle(apple);
	CloseHandle(orange);

	return 0;
}
