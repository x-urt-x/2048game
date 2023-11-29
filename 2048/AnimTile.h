#ifndef ANIMTILE_H
#define ANIMTILE_H

#include "Tile.h"

//перенести в tile.h ------------------------------------
struct Tile_point
{
	Point pointr, pointl;
	Tile* tile;
};

class AnimTile
{
public:
	AnimTile(std::vector<Tile_point> _row, int _from, int _to, int _dir, int num, bool megre, int _steps_between_tiles);
	bool move();
	void draw(sf::RenderTexture& texture);

private:
	void calcStep();
	void mergeAnim();
	Tile* tileToAnim; //элемент для анимации
	Tile* tileToSet; //конечный элемент, которому надо будет присвоить значение после анимации 
	std::vector<Tile_point> row;
	int from, to, dir;
	bool merge;
	int currentStep;
	float x_step, y_step;
	Point pos;
	int steps_between_tiles;
	sf::Texture texture;
};
#endif