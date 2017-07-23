#include "HRRN.h"

HRRN::HRRN()
{
}

HRRN::HRRN(int timeSlice)
{
	this->_timeSlice = timeSlice;
	this->_aviId = 1;

	//初始化临界区 
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
	//创建数据包
	Data3 * data = new Data3();
	data->point = this;
	data->missionLen = missionLen;

	_beginthread(ThreadAddMisssion, 0, data);
}

void HRRN::ThreadStart(LPVOID param)
{
	//强制类型装换
	HRRN * p = (HRRN*)param;

	while (1)
	{
		//这里不用加锁，不用担心检测不空，突然被别的线程清空Ram的情况，因为只有这个线程可以清除任务
		if (!p->_Ram.empty())		//有任务
		{
			//改变wait数据和扫描Run任务
			EnterCriticalSection(&(p->_mutex));
			int needToRun = -1;								//记录等下需要运行的任务,循环结束后还是-1的话证明当前没有任务在运行
			for (int i = p->_Ram.size() - 1; i >= 0; i--)
			{
				if (p->_Ram[i].state == MissionState::READY)
					p->_Ram[i].wait++;
				else if (p->_Ram[i].state == MissionState::RUN)
					needToRun = i;
			}
			LeaveCriticalSection(&(p->_mutex));

			Sleep(p->_timeSlice);							//任务运行一个时间片

			EnterCriticalSection(&(p->_mutex));
			if (needToRun == -1)	//当前没有运行任务
			{
				//执行高响应比算法
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

					//仅改变状态，并没有运行起来，下一次循环才运行
					p->_Ram[key].state = MissionState::RUN;
				}
			}
			else 
			{
				//运行那个处于运行状态的任务
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
	//强制类型装换
	HRRN * p = ((Data3*)param)->point;
	int len = ((Data3*)param)->missionLen;
	delete param;

	//创建一个任务
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.wait = 0;
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


void HRRN::ThreadDisPlayState(LPVOID param)
{
	//强制类型装换
	HRRN * p = (HRRN*)param;

	while (1)
	{
		system("cls");
		cout << "作业状态" << " | " << "作业ID" << " | " << "作业长度" << " | " << "进度" << " | " << "等待时间" << endl;

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

			cout << "		" << p->_Ram[i].id << "	" << p->_Ram[i].length << "      " << p->_Ram[i].progress << "      " << p->_Ram[i].wait << endl;
		}
		LeaveCriticalSection(&(p->_mutex));

		cout << "输入任务长度并回车即可添加新任务：" << endl;
	}
}


