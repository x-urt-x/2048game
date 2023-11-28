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
#include "AnimTile.h"


class Game
{
public:
	Game(StartParamsStruct& _StartParams);
	void Run();

private:
	void Render_GetZeros(std::vector<Tile_point*>& zeros);
	void newTile(std::vector<std::vector<Tile_point>> matr);
	bool move(int matrInd, int dir, bool v);
	void moveRow(int mi, int ri, int dir, bool v, bool* isEnd, bool* canMove, sf::RenderTexture* texture, bool* stop);
	std::vector<std::vector<std::vector<Tile_point>>> CreateMatrices();

	static float* maketrans(float bases[]);
	static Point newbase(float trans[], Point oldpoint);
	

	const StartParamsStruct& StartParams;
	std::vector<std::vector<std::vector<Tile_point>>> matrices;
	sf::RenderWindow window;
	sf::Text scores, msg;
	sf::Font font;
	int state, score = 0;
};
#endif