#ifndef GAME_H
#define GAME_H

#include <SFML/Graphics.hpp>
#include <vector>
#include <math.h>
#include <time.h>
#include "windows.h"
#include "Tile.h"
#include "StartParams.h"
#include <thread>
#include <chrono>

struct Tile_point
{
	Point pointr, pointl;
	Tile* tile;
};

class Game
{
public:
	Game(StartParamsStruct& _StartParams);
	void Run();

private:
	void Render_GetZeros(std::vector<Tile_point*>& zeros);
	void newTile(std::vector<std::vector<Tile_point>> matr);
	bool move(int matrInd, int dir, bool v);
	std::vector<std::vector<std::vector<Tile_point>>> CreateMatrices();
	static float* maketrans(float bases[]);
	static Point newbase(float trans[], Point oldpoint);
	

	const StartParamsStruct& StartParams;
	std::vector<std::vector<std::vector<Tile_point>>> matrices;
	sf::RenderWindow window;
	sf::Text scores, msg;
	sf::Font font;
	int state, score = 0;
	sf::Rect<int> field;
	std::vector<sf::ConvexShape> arrows;
};
#endif