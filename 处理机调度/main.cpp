//--------------------------------------------
/*Copyright (c) 2016 ÍõºéÌÎ rights reserved.*/
//--------------------------------------------
#include <iostream>
#include "RoundRobin.h"
#include "DyPriority.h"
#include "HRRN.h"
using namespace std;

int main()
{
// 	RoundRobin test(300);
// 	test.Start();
// 
// 	int i;
// 	while (cin >> i)
// 	{
// 		test.AddMisssion(i);
// 	}

// 	DyPriority test(800);
// 	test.Start();
// 
// 	int i, j;
// 	while (cin >> i >> j)
// 	{
// 		test.AddMisssion(i, j);
// 	}

	HRRN test(300);
	test.Start();

	int i;
	while (cin >> i)
	{
		test.AddMisssion(i);
	}

	return 0;
}
