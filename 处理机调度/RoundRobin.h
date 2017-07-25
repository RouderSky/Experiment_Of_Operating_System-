#include "StdAfx.h"
#include <windows.h>
#include <process.h>
#include <iostream>
#include <vector>
#include "Struct.h"
using namespace std;

class RoundRobin;
struct Data
{
	RoundRobin * point;
	int missionLen;
};

//一读多写型，一个mutex搞定

class RoundRobin
{
public:
	RoundRobin(int timeSlice);
	~RoundRobin();
	
	//处理机开始运行,在其中开启两个线程：一个线程用于处理调度，另一个线程用于显示当前处理机状态
	void Start();			

	//为处理机添加任务，在其中开启一个线程；该线程添加完任务后就被关闭；
	//添加任务后需要对index++，因为添加任务总是从左边添加；添加完成之后，被添加任务右边的任务都被向右挤了一格，index也要向右移动一格进行对齐；
	void AddMisssion(int missionLen);		

	//两个写者，一个读者
	static void ThreadStart(LPVOID param);
	static void ThreadDisPlayState(LPVOID param);		
	static void ThreadAddMisssion(LPVOID param);

private:
	RoundRobin();

	//处理机的时间片长度
	int timeSlice;

	//以下为共享资源，共享资源是多个线程可能同时访问的数据
	//但是Ram的每个元素不是共享资源，因为不可能有多个线程同时访问到同一任务

	//用于存储当前已录入任务
	//内存中的每一个任务都不是共享资源，因为每一任务在同一时间内只会被一个线程访问到
	vector<Mission> Ram;

	//用于记录当前正执行的任务，一定要和Ram保持同步;
	//相当于一个指针，会依次向右循环移动;用于模拟循环队列;
	//越靠近index左边的任务越接近于队尾，越靠近index右边的任务月接近于对头;
	int index;

	//当前可分配任务ID
	int aviId;

	//临界区
	CRITICAL_SECTION mutex;
};

