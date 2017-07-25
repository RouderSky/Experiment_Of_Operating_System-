#include "DyPriority.h"


DyPriority::DyPriority()
{
}

DyPriority::DyPriority(int timeSlice)
{
	this->_timeSlice = timeSlice;
	this->_aviId = 1;

	//初始化临界区 
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
	//创建数据包
	Data2 * data = new Data2();
	data->point = this;
	data->missionLen = missionLen;
	data->priority = priority;

	_beginthread(ThreadAddMisssion, 0, data);
}

void DyPriority::ThreadStart(LPVOID param)
{
	//强制类型装换
	DyPriority * p = (DyPriority*)param;

	while (1)
	{
		//这里不用加锁，不用担心检测不空，突然被别的线程清空Ram的情况，因为只有这个线程可以清除任务
		if (!p->_Ram.empty())		//有任务
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

				//刷掉所有的状态
				p->_Ram[i].state = MissionState::READY;
			}

			//优先级最高的变成运行状态
			p->_Ram[needToRun].state = MissionState::RUN;

			LeaveCriticalSection(&(p->_mutex));

			Sleep(p->_timeSlice);							//任务运行一个时间片

			EnterCriticalSection(&(p->_mutex));
			p->_Ram[needToRun].progress++;
			if (p->_Ram[needToRun].progress == p->_Ram[needToRun].length)
			{
				//已完成的直接删除
				p->_Ram[needToRun].state = MissionState::FNISH;
				p->_Ram.erase(p->_Ram.begin() + needToRun);
			}
				
			LeaveCriticalSection(&(p->_mutex));
		}
	}
}

void DyPriority::ThreadAddMisssion(LPVOID param)
{
	//强制类型装换
	DyPriority * p = ((Data2*)param)->point;
	int len = ((Data2*)param)->missionLen;
	int prio = ((Data2*)param)->priority;
	delete param;

	//创建一个任务
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.priority = prio;
	mission.state = MissionState::READY;


	//这个地方要加一把锁，不然都一起来会乱
	//写
	EnterCriticalSection(&(p->_mutex));
	mission.id = p->_aviId;
	p->_aviId++;

	p->_Ram.push_back(mission);
	LeaveCriticalSection(&(p->_mutex));
	//

}


void DyPriority::ThreadDisPlayState(LPVOID param)
{
	//强制类型装换
	DyPriority * p = (DyPriority*)param;

	while (1)
	{
		system("cls");
		cout << "作业状态" << " | " << "作业ID" << " | " << "作业长度" << " | " << "进度" << " | " << "优先级" << endl;

		//读
		EnterCriticalSection(&(p->_mutex));
		for (int i = 0; i < p->_Ram.size(); i++)
		{
			//改改输出的格式
			cout << "  ";
			if (p->_Ram[i].state == MissionState::READY)
				cout << "就绪";
			else if (p->_Ram[i].state == MissionState::RUN)
				cout << "运行";
			else
				cout << "完成";

			cout << "		" << p->_Ram[i].id << "	" << p->_Ram[i].length << "      " << p->_Ram[i].progress << "      " << p->_Ram[i].priority << endl;
		}
		LeaveCriticalSection(&(p->_mutex));

		cout << "输入任务长度以及任务优先级并回车即可添加新任务：" << endl;
	}
}


