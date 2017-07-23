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

	//�������ʼ����,�����п��������̣߳�һ���߳����ڴ�����ȣ���һ���߳�������ʾ��ǰ�����״̬
	void Start();

	//Ϊ�����������������п���һ���̣߳����߳�����������ͱ��رգ�
	void AddMisssion(int missionLen);

	//����д�ߣ�һ������
	static void ThreadStart(LPVOID param);
	static void ThreadDisPlayState(LPVOID param);
	static void ThreadAddMisssion(LPVOID param);

private:
	HRRN();

	//�������ʱ��Ƭ����
	int _timeSlice;

	//�洢��ǰ�ڴ��е�����
	vector<Mission> _Ram;

	//��ǰ�ɷ�������ID
	int _aviId;

	//�ٽ���
	CRITICAL_SECTION _mutex;
};

