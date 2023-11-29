#include <iostream>
#include "StartParams.h"
#include "Game.h"



int main()
{
	StartParamsStruct StartParams;

	StartParams.axis = 4;
	StartParams.w_x = 1400;
	StartParams.w_y = 1400;
	StartParams.spawn_s_q = 3;
	StartParams.spawn_q = 0;
	StartParams.spawn_r = 2;
	StartParams.a = 5;
	StartParams.base = 2;
	StartParams.max_degree = 99;
	StartParams.speed = 0.5;
	StartParams.stepsPerSec = 30;


	//std::cin >> StartParams.a;
	int min = StartParams.w_x > StartParams.w_y ? StartParams.w_y : StartParams.w_x;
	StartParams.range = min / (StartParams.a * 2 - 1);
	StartParams.r = StartParams.range * sin(0.5/StartParams.axis*3.14);

	Game game(StartParams);
	game.Run();
	
	return 0;
}