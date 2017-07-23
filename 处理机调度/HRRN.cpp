#include "HRRN.h"

HRRN::HRRN()
{
}

HRRN::HRRN(int timeSlice)
{
	this->_timeSlice = timeSlice;
	this->_aviId = 1;

	//��ʼ���ٽ��� 
	InitializeCriticalSection(&_mutex);
}


HRRN::~HRRN()
{
	DeleteCriticalSection(&_mutex);
}

void HRRN::Start()
{
	_beginthread(ThreadStart, 0, this);
	_beginthread(ThreadDisPlayState, 0, this);
}

void HRRN::AddMisssion(int missionLen)
{
	//�������ݰ�
	Data3 * data = new Data3();
	data->point = this;
	data->missionLen = missionLen;

	_beginthread(ThreadAddMisssion, 0, data);
}

void HRRN::ThreadStart(LPVOID param)
{
	//ǿ������װ��
	HRRN * p = (HRRN*)param;

	while (1)
	{
		//���ﲻ�ü��������õ��ļ�ⲻ�գ�ͻȻ������߳����Ram���������Ϊֻ������߳̿����������
		if (!p->_Ram.empty())		//������
		{
			//�ı�wait���ݺ�ɨ��Run����
			EnterCriticalSection(&(p->_mutex));
			int needToRun = -1;								//��¼������Ҫ���е�����,ѭ����������-1�Ļ�֤����ǰû������������
			for (int i = p->_Ram.size() - 1; i >= 0; i--)
			{
				if (p->_Ram[i].state == MissionState::READY)
					p->_Ram[i].wait++;
				else if (p->_Ram[i].state == MissionState::RUN)
					needToRun = i;
			}
			LeaveCriticalSection(&(p->_mutex));

			Sleep(p->_timeSlice);							//��������һ��ʱ��Ƭ

			EnterCriticalSection(&(p->_mutex));
			if (needToRun == -1)	//��ǰû����������
			{
				//ִ�и���Ӧ���㷨
				if (!p->_Ram.empty())
				{
					float hr = 1 + (p->_Ram[0].wait / p->_Ram[0].length);
					int key = 0;
					for (int i = 1; i < p->_Ram.size(); i++)
					{
						float temp = 1 + ((float)p->_Ram[i].wait / p->_Ram[i].length);
						if (temp > hr)
						{
							hr = temp;
							key = i;
						}
					}

					//���ı�״̬����û��������������һ��ѭ��������
					p->_Ram[key].state = MissionState::RUN;
				}
			}
			else 
			{
				//�����Ǹ���������״̬������
				p->_Ram[needToRun].progress++;
				if (p->_Ram[needToRun].progress == p->_Ram[needToRun].length)
				{
					p->_Ram[needToRun].state == MissionState::FNISH;
					p->_Ram.erase(p->_Ram.begin() + needToRun);
				}
			}
			LeaveCriticalSection(&(p->_mutex));
		}
	}
}

void HRRN::ThreadAddMisssion(LPVOID param)
{
	//ǿ������װ��
	HRRN * p = ((Data3*)param)->point;
	int len = ((Data3*)param)->missionLen;
	delete param;

	//����һ������
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.wait = 0;
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


void HRRN::ThreadDisPlayState(LPVOID param)
{
	//ǿ������װ��
	HRRN * p = (HRRN*)param;

	while (1)
	{
		system("cls");
		cout << "��ҵ״̬" << " | " << "��ҵID" << " | " << "��ҵ����" << " | " << "����" << " | " << "�ȴ�ʱ��" << endl;

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

			cout << "		" << p->_Ram[i].id << "	" << p->_Ram[i].length << "      " << p->_Ram[i].progress << "      " << p->_Ram[i].wait << endl;
		}
		LeaveCriticalSection(&(p->_mutex));

		cout << "�������񳤶Ȳ��س��������������" << endl;
	}
}


