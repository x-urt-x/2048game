#include <iostream>
#include "StartParams.h"
#include "Game.h"



int main()
{
	StartParamsStruct StartParams;

	StartParams.axis = 4;
	StartParams.w_x = 1400;
	StartParams.w_y = 1400;
	StartParams.spawn_s_q = 4;
	StartParams.spawn_q = 10;
	StartParams.spawn_r = 3;
	StartParams.a = 4;
	StartParams.base = 2;
	StartParams.max_degree = 99;
	StartParams.r = (StartParams.w_x / 2 - StartParams.w_x / 10) / (StartParams.a * 2 - 1);
	//StartParams.r = 20;
	StartParams.range = StartParams.r * 2;

	int min = StartParams.w_x > StartParams.w_y ? StartParams.w_y : StartParams.w_x;
	StartParams.r = min / (StartParams.a * 2 - 1) / 2;
	StartParams.range = StartParams.r * 0.1;

	Game game(StartParams);
	game.Run();
	
	return 0;
}