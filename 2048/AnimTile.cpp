#include "AnimTile.h"



AnimTile::AnimTile(std::vector<Tile_point> _row, int _from, int _to, int _dir, int _num, bool _merge, int _steps_between_tiles)
{
	tileToSet = _row[_to].tile;
	row = _row;
	from = _from;
	to = _to;
	merge = _merge;
	dir = _dir;
	currentStep = 0;
	steps_between_tiles = _steps_between_tiles;
	calcStep();
	tileToAnim = new Tile(pos);
	tileToAnim->setNum(_num);
	pos = row[from].pointr;
}

void AnimTile::calcStep()
{
	if (from == to)
	{
		x_step = 0;
		y_step = 0;
	}
	else
	{
	x_step = (row[from - dir].pointr.x - row[from].pointr.x) / steps_between_tiles;
	y_step = (row[from - dir].pointr.y - row[from].pointr.y) / steps_between_tiles;
	}
}



bool AnimTile::move()
{
	if (from == to)
	{
		if (merge)
		{
			mergeAnim();
			tileToSet->setNum(tileToAnim->getNum() * 2);
		}
		else
		{
			tileToSet->setNum(tileToAnim->getNum());
		}
		return true;
	}
	if (currentStep >= steps_between_tiles)
	{
		currentStep = 0;
		from -= dir;
		calcStep();
	}
	pos.x += x_step;
	pos.y += y_step;
	tileToAnim->setPos(pos);
	currentStep++;
	return false;
}

void AnimTile::mergeAnim()
{
}

void AnimTile::draw(sf::RenderTexture& texture)
{
	tileToAnim->DrawTexture(texture);
}
