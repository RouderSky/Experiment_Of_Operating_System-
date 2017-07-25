#include "StdAfx.h"
#include <windows.h>
#include <process.h>
#include <iostream>
#include <vector>
#include "Struct.h"
using namespace std;



class HRRN;
struct Data3
{
	HRRN * point;
	int missionLen;
};


class HRRN
{
public:
	HRRN(int timeSlice);
	~HRRN();

	//处理机开始运行,在其中开启两个线程：一个线程用于处理调度，另一个线程用于显示当前处理机状态
	void Start();

	//为处理机添加任务，在其中开启一个线程；该线程添加完任务后就被关闭；
	void AddMisssion(int missionLen);

	//两个写者，一个读者
	static void ThreadStart(LPVOID param);
	static void ThreadDisPlayState(LPVOID param);
	static void ThreadAddMisssion(LPVOID param);

private:
	HRRN();

	//处理机的时间片长度
	int _timeSlice;

	//存储当前内存中的任务
	vector<Mission> _Ram;

	//当前可分配任务ID
	int _aviId;

	//临界区
	CRITICAL_SECTION _mutex;
};

