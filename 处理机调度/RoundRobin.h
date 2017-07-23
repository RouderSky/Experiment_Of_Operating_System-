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

//һ����д�ͣ�һ��mutex�㶨

class RoundRobin
{
public:
	RoundRobin(int timeSlice);
	~RoundRobin();
	
	//�������ʼ����,�����п��������̣߳�һ���߳����ڴ�����ȣ���һ���߳�������ʾ��ǰ�����״̬
	void Start();			

	//Ϊ�����������������п���һ���̣߳����߳�����������ͱ��رգ�
	//����������Ҫ��index++����Ϊ����������Ǵ������ӣ�������֮�󣬱���������ұߵ����񶼱����Ҽ���һ��indexҲҪ�����ƶ�һ����ж��룻
	void AddMisssion(int missionLen);		

	//����д�ߣ�һ������
	static void ThreadStart(LPVOID param);
	static void ThreadDisPlayState(LPVOID param);		
	static void ThreadAddMisssion(LPVOID param);

private:
	RoundRobin();

	//�������ʱ��Ƭ����
	int timeSlice;

	//����Ϊ������Դ��������Դ�Ƕ���߳̿���ͬʱ���ʵ�����
	//����Ram��ÿ��Ԫ�ز��ǹ�����Դ����Ϊ�������ж���߳�ͬʱ���ʵ�ͬһ����

	//���ڴ洢��ǰ��¼������
	//�ڴ��е�ÿһ�����񶼲��ǹ�����Դ����Ϊÿһ������ͬһʱ����ֻ�ᱻһ���̷߳��ʵ�
	vector<Mission> Ram;

	//���ڼ�¼��ǰ��ִ�е�����һ��Ҫ��Ram����ͬ��;
	//�൱��һ��ָ�룬����������ѭ���ƶ�;����ģ��ѭ������;
	//Խ����index��ߵ�����Խ�ӽ��ڶ�β��Խ����index�ұߵ������½ӽ��ڶ�ͷ;
	int index;

	//��ǰ�ɷ�������ID
	int aviId;

	//�ٽ���
	CRITICAL_SECTION mutex;
};

