#include "StdAfx.h"
#include <windows.h>
#include <process.h>
#include <iostream>
#include <vector>
#include "Struct.h"
using namespace std;



class DyPriority;
struct Data2
{
	DyPriority * point;
	int missionLen;
	int priority;
};


class DyPriority
{
public:
	DyPriority(int timeSlice);
	~DyPriority();

	//�������ʼ����,�����п��������̣߳�һ���߳����ڴ�����ȣ���һ���߳�������ʾ��ǰ�����״̬
	void Start();

	//Ϊ�����������������п���һ���̣߳����߳�����������ͱ��رգ�
	void AddMisssion(int missionLen,int priority);

	//����д�ߣ�һ������
	static void ThreadStart(LPVOID param);
	static void ThreadDisPlayState(LPVOID param);
	static void ThreadAddMisssion(LPVOID param);

private:
	DyPriority();

	//�������ʱ��Ƭ����
	int _timeSlice;

	//�洢��ǰ�ڴ��е�����
	vector<Mission> _Ram;

	//��ǰ�ɷ�������ID
	int _aviId;

	//�ٽ���
	CRITICAL_SECTION _mutex;
};
