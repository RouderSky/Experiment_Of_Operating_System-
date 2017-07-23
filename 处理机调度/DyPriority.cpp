#include "DyPriority.h"


DyPriority::DyPriority()
{
}

DyPriority::DyPriority(int timeSlice)
{
	this->_timeSlice = timeSlice;
	this->_aviId = 1;

	//��ʼ���ٽ��� 
	InitializeCriticalSection(&_mutex);
}


DyPriority::~DyPriority()
{
	DeleteCriticalSection(&_mutex);
}

void DyPriority::Start()
{
	_beginthread(ThreadStart, 0, this);
	_beginthread(ThreadDisPlayState, 0, this);
}

void DyPriority::AddMisssion(int missionLen, int priority)
{
	//�������ݰ�
	Data2 * data = new Data2();
	data->point = this;
	data->missionLen = missionLen;
	data->priority = priority;

	_beginthread(ThreadAddMisssion, 0, data);
}

void DyPriority::ThreadStart(LPVOID param)
{
	//ǿ������װ��
	DyPriority * p = (DyPriority*)param;

	while (1)
	{
		//���ﲻ�ü��������õ��ļ�ⲻ�գ�ͻȻ������߳����Ram���������Ϊֻ������߳̿����������
		if (!p->_Ram.empty())		//������
		{
			EnterCriticalSection(&(p->_mutex));
			int needToRun = p->_Ram.size() - 1;
			for (int i = p->_Ram.size()-1; i >= 0; i--)
			{
				if (p->_Ram[i].state == MissionState::READY)
				{
					p->_Ram[i].priority++;
					if (p->_Ram[i].priority > p->_Ram[needToRun].priority)
						needToRun = i;
				}
				else if (p->_Ram[i].state == MissionState::RUN)
				{
					p->_Ram[i].priority--;
					if (p->_Ram[i].priority > p->_Ram[needToRun].priority)
						needToRun = i;
				}

				//ˢ�����е�״̬
				p->_Ram[i].state = MissionState::READY;
			}

			//���ȼ���ߵı������״̬
			p->_Ram[needToRun].state = MissionState::RUN;

			LeaveCriticalSection(&(p->_mutex));

			Sleep(p->_timeSlice);							//��������һ��ʱ��Ƭ

			EnterCriticalSection(&(p->_mutex));
			p->_Ram[needToRun].progress++;
			if (p->_Ram[needToRun].progress == p->_Ram[needToRun].length)
			{
				//����ɵ�ֱ��ɾ��
				p->_Ram[needToRun].state = MissionState::FNISH;
				p->_Ram.erase(p->_Ram.begin() + needToRun);
			}
				
			LeaveCriticalSection(&(p->_mutex));
		}
	}
}

void DyPriority::ThreadAddMisssion(LPVOID param)
{
	//ǿ������װ��
	DyPriority * p = ((Data2*)param)->point;
	int len = ((Data2*)param)->missionLen;
	int prio = ((Data2*)param)->priority;
	delete param;

	//����һ������
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.priority = prio;
	mission.state = MissionState::READY;


	//����ط�Ҫ��һ��������Ȼ��һ��������
	//д
	EnterCriticalSection(&(p->_mutex));
	mission.id = p->_aviId;
	p->_aviId++;

	p->_Ram.push_back(mission);
	LeaveCriticalSection(&(p->_mutex));
	//

}


void DyPriority::ThreadDisPlayState(LPVOID param)
{
	//ǿ������װ��
	DyPriority * p = (DyPriority*)param;

	while (1)
	{
		system("cls");
		cout << "��ҵ״̬" << " | " << "��ҵID" << " | " << "��ҵ����" << " | " << "����" << " | " << "���ȼ�" << endl;

		//��
		EnterCriticalSection(&(p->_mutex));
		for (int i = 0; i < p->_Ram.size(); i++)
		{
			//�ĸ�����ĸ�ʽ
			cout << "  ";
			if (p->_Ram[i].state == MissionState::READY)
				cout << "����";
			else if (p->_Ram[i].state == MissionState::RUN)
				cout << "����";
			else
				cout << "���";

			cout << "		" << p->_Ram[i].id << "	" << p->_Ram[i].length << "      " << p->_Ram[i].progress << "      " << p->_Ram[i].priority << endl;
		}
		LeaveCriticalSection(&(p->_mutex));

		cout << "�������񳤶��Լ��������ȼ����س��������������" << endl;
	}
}


