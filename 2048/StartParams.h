#ifndef StartParams_H
#define StartParams_H
#include <SFML/Graphics.hpp>
const struct StartParamsStruct
{
	int axis, w_x, w_y, spawn_s_q, spawn_q, spawn_r, a, base, max_degree, r, range;
	sf::Rect<int> field;
};
#endif