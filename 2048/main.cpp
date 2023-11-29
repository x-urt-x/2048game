#include <iostream>
#include "StartParams.h"
#include "Game.h"



int main()
{
	StartParamsStruct StartParams;

	StartParams.axis = 4;
	StartParams.w_x = 1200;
	StartParams.w_y = 1200;
	StartParams.spawn_s_q = 3;
	StartParams.spawn_q = 2;
	StartParams.spawn_r = 4;
	StartParams.a = 4;
	StartParams.base = 2;
	StartParams.max_degree = 99;



	StartParams.field.left = StartParams.w_x * 0.05;
	StartParams.field.top = StartParams.w_y * 0.05;
	StartParams.field.width = StartParams.w_x - 2 * StartParams.field.left;
	StartParams.field.height = StartParams.w_y - 2 * StartParams.field.top;

	int min = StartParams.field.width > StartParams.field.height ? StartParams.field.height : StartParams.field.width;
	StartParams.range = min / (StartParams.a * 2 - 1);
	StartParams.r = StartParams.range * sin(0.5/StartParams.axis*3.14);

	Game game(StartParams);
	game.Run();
	
	return 0;
}