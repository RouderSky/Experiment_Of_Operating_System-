#include "RoundRobin.h"

RoundRobin::RoundRobin()
{
}

RoundRobin::RoundRobin(int timeSlice)
{
	this->timeSlice = timeSlice;			
	this->aviId = 1;
	this->index = -1;

	//��ʼ���ٽ��� 
	InitializeCriticalSection(&mutex);
}


RoundRobin::~RoundRobin()
{
	DeleteCriticalSection(&mutex);
}

void RoundRobin::Start()
{
	_beginthread(ThreadStart, 0, this);
	_beginthread(ThreadDisPlayState, 0, this);
}

void RoundRobin::AddMisssion(int missionLen)
{
	//�������ݰ�
	Data * data = new Data();
	data->point = this;
	data->missionLen = missionLen;

	_beginthread(ThreadAddMisssion, 0, data);
}

void RoundRobin::ThreadStart(LPVOID param)
{
	//ǿ������װ��
	RoundRobin * p = (RoundRobin*)param;

	while (1)
	{
		//���ﲻ�ü��������õ��ļ�ⲻ�գ�ͻȻ������߳����Ram���������Ϊֻ������߳̿����������
		if (!p->Ram.empty())		//������
		{
			//���Ǹ���index����Ram����䶼Ҫ��������Ϊ�������ʱҪȷ��������ͬ���仯��

			EnterCriticalSection(&(p->mutex));				
			p->Ram[p->index].state = MissionState::RUN;		//��������״̬
			LeaveCriticalSection(&(p->mutex));				

			Sleep(p->timeSlice);							//��������һ��ʱ��Ƭ

			EnterCriticalSection(&(p->mutex));
			p->Ram[p->index].progress++;					//���ȼ�һ��ע�⣺�������Ҫ����ȡ������Ϊ�����ǡ�������������������һ��ʱ��Ƭ����++

			//����״̬����
			if (p->Ram[p->index].progress == p->Ram[p->index].length)		//����������
			{
				p->Ram[p->index].state = MissionState::FNISH;

				//д
//				EnterCriticalSection(&(p->mutex));
				//��Ҫ�����������
				p->Ram.erase(p->Ram.begin() + p->index);
				p->index--;
//				LeaveCriticalSection(&(p->mutex));
			}
			else															//�������û�����
			{
				p->Ram[p->index].state = MissionState::READY;
			}

			//index�л�����һ������
			//д
//			EnterCriticalSection(&(p->mutex));
			if (!p->Ram.empty())
			{
				p->index++;
				p->index %= p->Ram.size();
			}
			LeaveCriticalSection(&(p->mutex));
		}

		
	}
}

void RoundRobin::ThreadAddMisssion(LPVOID param)
{
	//ǿ������װ��
	RoundRobin * p = ((Data*)param)->point;
	int len = ((Data*)param)->missionLen;
	delete param;

	//����һ������
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.state = MissionState::READY;


	//����ط�Ҫ��һ��������Ȼ��һ��������
	//д
	EnterCriticalSection(&(p->mutex));
	mission.id = p->aviId;
	p->aviId++;

//	p->Ram.push_back(mission);
	if (p->Ram.empty())
		p->Ram.insert(p->Ram.begin(), mission);
	else
		p->Ram.insert(p->Ram.begin() + p->index, mission);
	p->index++;
	LeaveCriticalSection(&(p->mutex));
	//
	
}


void RoundRobin::ThreadDisPlayState(LPVOID param)
{
	//ǿ������װ��
	RoundRobin * p = (RoundRobin*)param;

	while (1)
	{
		system("cls");
		cout << "��ҵ״̬" << " | " << "��ҵID" << " | " << "��ҵ����" << " | " << "����" << endl;

		//��
		EnterCriticalSection(&(p->mutex));
		for (int i = 0; i < p->Ram.size(); i++)
		{
			//�ĸ�����ĸ�ʽ
			cout << "  ";
			if (p->Ram[i].state == MissionState::READY)
				cout << "����";
			else if (p->Ram[i].state == MissionState::RUN)
				cout << "����";
			else
				cout << "���";

			cout << "		" << p->Ram[i].id << "	" << p->Ram[i].length << "      " << p->Ram[i].progress << endl;
		}
		LeaveCriticalSection(&(p->mutex));

		cout << "�������񳤶Ȳ��س��������������" << endl;
	}
}


