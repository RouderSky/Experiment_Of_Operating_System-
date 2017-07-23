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
	int priority;			//��ֵԽ�����ȼ�Խ��
	int id;
	int length;				//��λ�ǣ���ʱ��Ƭ
	int progress;			//��λ�ǣ���ʱ��Ƭ
	int wait;				//��λ�ǣ���ʱ��Ƭ
};

#endif
