#include "RoundRobin.h"

RoundRobin::RoundRobin()
{
}

RoundRobin::RoundRobin(int timeSlice)
{
	this->timeSlice = timeSlice;			
	this->aviId = 1;
	this->index = -1;

	//初始化临界区 
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
	//创建数据包
	Data * data = new Data();
	data->point = this;
	data->missionLen = missionLen;

	_beginthread(ThreadAddMisssion, 0, data);
}

void RoundRobin::ThreadStart(LPVOID param)
{
	//强制类型装换
	RoundRobin * p = (RoundRobin*)param;

	while (1)
	{
		//这里不用加锁，不用担心检测不空，突然被别的线程清空Ram的情况，因为只有这个线程可以清除任务
		if (!p->Ram.empty())		//有任务
		{
			//凡是根据index访问Ram的语句都要加锁，因为添加任务时要确保他们是同步变化的

			EnterCriticalSection(&(p->mutex));				
			p->Ram[p->index].state = MissionState::RUN;		//更改任务状态
			LeaveCriticalSection(&(p->mutex));				

			Sleep(p->timeSlice);							//任务运行一个时间片

			EnterCriticalSection(&(p->mutex));
			p->Ram[p->index].progress++;					//进度加一，注意：这个进度要向下取整，因为精度是“个”，所以是运行完一个时间片才能++

			//任务状态调整
			if (p->Ram[p->index].progress == p->Ram[p->index].length)		//如果任务完成
			{
				p->Ram[p->index].state = MissionState::FNISH;

				//写
//				EnterCriticalSection(&(p->mutex));
				//需要从数组中清除
				p->Ram.erase(p->Ram.begin() + p->index);
				p->index--;
//				LeaveCriticalSection(&(p->mutex));
			}
			else															//如果任务没有完成
			{
				p->Ram[p->index].state = MissionState::READY;
			}

			//index切换到下一个任务
			//写
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
	//强制类型装换
	RoundRobin * p = ((Data*)param)->point;
	int len = ((Data*)param)->missionLen;
	delete param;

	//创建一个任务
	Mission mission;
	mission.length = len;
	mission.progress = 0;
	mission.state = MissionState::READY;


	//这个地方要加一把锁，不然都一起来会乱
	//写
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
	//强制类型装换
	RoundRobin * p = (RoundRobin*)param;

	while (1)
	{
		system("cls");
		cout << "作业状态" << " | " << "作业ID" << " | " << "作业长度" << " | " << "进度" << endl;

		//读
		EnterCriticalSection(&(p->mutex));
		for (int i = 0; i < p->Ram.size(); i++)
		{
			//改改输出的格式
			cout << "  ";
			if (p->Ram[i].state == MissionState::READY)
				cout << "就绪";
			else if (p->Ram[i].state == MissionState::RUN)
				cout << "运行";
			else
				cout << "完成";

			cout << "		" << p->Ram[i].id << "	" << p->Ram[i].length << "      " << p->Ram[i].progress << endl;
		}
		LeaveCriticalSection(&(p->mutex));

		cout << "输入任务长度并回车即可添加新任务：" << endl;
	}
}


