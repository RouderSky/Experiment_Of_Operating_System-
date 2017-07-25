#ifndef __STRUCT_H__
#define __STRUCT_H__

enum MissionState
{
	READY,
	RUN,
	FNISH
};

struct Mission
{
	MissionState state;
	int priority;			//数值越大，优先级越高
	int id;
	int length;				//单位是：个时间片
	int progress;			//单位是：个时间片
	int wait;				//单位是：个时间片
};

#endif
