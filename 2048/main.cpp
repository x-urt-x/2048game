#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include "windows.h"

#include <chrono>
#include <thread>


using namespace sf;


Font font;
const int w_x = 1200, w_y = 1200;
const int spawn_s_q = 10;
const int spawn_q = 2;
const int spawn_r = 2;
const int a = 5;
const int base = 2;
const int max_degree = 11;
//const int r = (w_x / 2 - w_x / 10) / (a * 2 - 1);
const int r = 25;
int score = 0;
const Color tile_col[] =
{
	Color(97, 77, 60),
	Color(250,231,224),
	Color(249,225,206),
	Color(249,179,124),
	Color(246,149,96),
	Color(243,125,95),
	Color(254,92,65),
	Color(245,208,119),
	Color(249,208,103),
	Color(249,202,88),
	Color(239,196,42),
	Color(244,198,42)
};

RenderWindow window(VideoMode(w_x, w_y), "2048 game");

class Tile : public sf::CircleShape
{

public:
	Tile() {}
	Tile(int x, int y, int r)

		:CircleShape(r, 6)
	{
		this->setPosition(x, y);
		text_num.setCharacterSize(r / 2);
		text_num.setPosition(x + r * 0.866 - r / 4, y + r / 2);
		text_num.setFont(font);
		window.draw(*this);
		num = 0;
		upNum();
	}
	void setNum(int numToSet)
	{
		num = numToSet;
		upNum();
	}
	int getNum() { return num; }

	void upNum()
	{
		text_num.setString(std::to_string(num));
		if (num == 0)
			text_num.setFillColor(tile_col[0]);
		else if (num <= base * 2)
			text_num.setFillColor(Color(121, 112, 100));
		else
			text_num.setFillColor(Color::White);
		this->setFillColor(tile_col[int(num == 0 ? 0 : (log(num) / log(2) > max_degree ? max_degree : log(num) / log(2)))]);
		window.draw(*this);
		window.draw(text_num);
	}
private:
	int num = 0;
	Text text_num;
};

void newTile(std::vector < std::vector<Tile*>*>& matr)
{
	int r_x, r_y;
	do
	{
		r_x = rand() % (2 * a - 1);
		r_y = rand() % (2 * a - abs(a - 1 - r_x) - 1);
	} while (matr.at(r_x)->at(r_y)->getNum() != 0);
	matr.at(r_x)->at(r_y)->setNum(base * pow(2, (rand()) % spawn_r));
}


int move(std::vector < std::vector<Tile*>*>& matr, int dir, bool v)
{
	int moved = 0;
	std::vector<int> ind;
	for (int i = 0; i < 2 * a - 1; i++) { ind.push_back(i); }
	for (int i = 0; ind.size() > 0; i++)
	{
		for (int j = 0; j < ind.size(); j++)
		{
			int i_w = dir == 1 ? i : matr.at(ind[j])->size() - i - 1;
			bool flag_c = false;
			int num = matr.at(ind[j])->at(i_w)->getNum();
			int k_max = (2 * a - abs(a - 1 - ind[j]) - 1);
			if (num == 0)
			{
				for (int k = i_w + dir; dir == 1 ? k < k_max : k >= 0; k += dir)
				{
					if (matr.at(ind[j])->at(k)->getNum() != 0)
					{
						moved = 1;
						if (!v)
						{
							matr.at(ind[j])->at(i_w)->setNum(matr.at(ind[j])->at(k)->getNum());
							matr.at(ind[j])->at(k)->setNum(0);
							num = matr.at(ind[j])->at(i_w)->getNum();
							break;
						}
					}
					if (((k == k_max - 1) && (dir == 1)) || ((dir == -1) && (k == 0)))
					{
						ind.erase(ind.begin() + j);
						j--;
						flag_c = true;
						break;
					}
				}
			}
			for (int k = i_w + dir; (dir == 1 ? k < k_max : k >= 0) and !flag_c; k += dir)
			{
				if (matr.at(ind[j])->at(k)->getNum() == num)
				{
					moved = 1;
					if (!v)
					{
						matr.at(ind[j])->at(k)->setNum(0);
						matr.at(ind[j])->at(i_w)->setNum(num * 2);
						score += num * 2;
						break;
					}
				}
				else
				{
					if (matr.at(ind[j])->at(k)->getNum() != 0) break;
				}
			}
			if (((i_w == k_max - 1) && (dir == 1)) || ((dir == -1) && (i_w == 0)))
			{
				ind.erase(ind.begin() + j);
				j--;
			}
		}
	}
	return moved;
}


struct Point
{
	float x, y;
};

struct Tile_point
{
	Point point;
	Tile* tile;
};

float* maketrans(float* bases)
{
	float* trans = new float[4];

	if (bases[0] == 0)
	{
		float a;
		bases[0] = bases[1];
		a = bases[2];
		bases[2] = bases[3];
		bases[3] = a;
		a = bases[4];
		bases[4] = bases[5];
		bases[5] = a;
		a = bases[6];
		bases[6] = bases[7];
		bases[7] = a;
	}

	//делим первую строку на 7
	bases[2] /= bases[0];
	bases[4] /= bases[0];
	bases[6] /= bases[0];
	if (bases[0] != 0)
	{
		//вычитаем из второй первую с коэф. после этого в bases[1] будет 0
		bases[3] -= bases[2] * bases[1];
		bases[5] -= bases[4] * bases[1];
		bases[7] -= bases[6] * bases[1];
	}
	trans[1] = bases[5] / bases[3];
	trans[3] = bases[7] / bases[3];
	trans[0] = bases[4] - trans[1] * bases[2];
	trans[2] = bases[6] - trans[3] * bases[2];
	return trans;
}

Point newbase(float* trans, Point oldpoint)
{
	Point newpoint;
	newpoint.x = trans[0] * oldpoint.x + trans[2] * oldpoint.y;
	newpoint.y = trans[1] * oldpoint.x + trans[3] * oldpoint.y;
	return newpoint;
}

void addnewpoints(std::vector<Point>* newpoints, std::vector<Point> oldpoints, float* bases)
{
	float* trans = maketrans(bases);
	for (int i = 0; i < oldpoints.size(); i++)
	{
		newpoints->push_back(newbase(trans, oldpoints[i]));
	}
}


int main()
{

	bool lose = true, stop = false;
	int moved = 0, num = 0, won = 0;
	srand(time(NULL));
	font.loadFromFile("arial.ttf");
	window.setVerticalSyncEnabled(true);
	Text scores, msg;
	scores.setFont(font);
	msg.setFont(font);
	scores.setPosition(w_x / 2 - w_x / 10, w_y / 20);
	msg.setPosition(w_x / 20, w_y / 2 - w_y / 10);
	scores.setCharacterSize(w_y / 20);
	msg.setCharacterSize(w_y / 5);
	msg.setFillColor(Color::Red);


	int n = 3;
	int aa = 2;
	//std::cin >> n;
	float ang = 360.0 / (2 * n);




	std::vector<Point> points, recpoints;
	for (int i = 0; i < aa; i++)
	{
		for (int j = 1; j < aa - i; j++)
		{
			Point p = { i,j };
			points.push_back(p);
			Point p1 = { -i,-j };
			points.push_back(p1);
		}
	}

	float bases[8];
	//bases[0] = cos(0);
	//bases[1] = sin(0);
	//bases[2] = cos(ang);
	//bases[3] = sin(ang);
	bases[0] = 1;
	bases[1] = 0;
	bases[2] = 0;
	bases[3] = 1;
	for (int i = 0; i < n; i++)
	{
		bases[4] = cos((i + 1) * ang * 3.14 / 180);
		bases[5] = sin((i + 1) * ang * 3.14 / 180);
		bases[6] = cos(i * ang * 3.14 / 180);
		bases[7] = sin(i * ang * 3.14 / 180);
		addnewpoints(&recpoints, points, bases);
	}


	font.loadFromFile("arial.ttf");
	window.setVerticalSyncEnabled(true);
	std::vector<Tile_point> rectiles;
	for (int i = 0; i < recpoints.size(); i++)
	{
		rectiles.push_back(
			Tile_point
			{
				Point{recpoints[i].x,recpoints[i].y},
				new Tile(recpoints[i].x * 70 + 500, -(recpoints[i].y * 70) + 500, r)
			}
		);
	}
	bases[4] = 1;
	bases[5] = 0;
	bases[6] = 0;
	bases[7] = 1;
	std::vector<std::vector<Tile_point>> axeslists;
	for (int k = 0; k < n; k++)
	{
		bases[0] = cos((k + 1) * ang * 3.14 / 180);
		bases[1] = sin((k + 1) * ang * 3.14 / 180);
		bases[2] = cos(k * ang * 3.14 / 180);
		bases[3] = sin(k * ang * 3.14 / 180);
		float* trans = maketrans(bases);
		std::vector<Tile_point> tiles;
		for (int i = 0; i < rectiles.size(); i++)
		{
			tiles.push_back(
				Tile_point
				{
					newbase(trans, rectiles[i].point),
					rectiles[i].tile
				}
			);
		}
		axeslists.push_back(tiles);
	}


	bases[0] = 1;
	bases[1] = 0;
	bases[2] = 0;
	bases[3] = 1;

	bases[4] = cos( ang * 3.14 / 180);
	bases[5] = sin( ang * 3.14 / 180);
	bases[6] = 1;
	bases[7] = 0;
	float* trans = maketrans(bases);
	std::vector<CircleShape*> clist;
	for (int i = 0; i < axeslists[0].size(); i++)
	{
		Point p1 = newbase(trans, axeslists[0][i].point);
		CircleShape* c = new sf::CircleShape(25);
		c->setPosition(p1.x * 70 + 500, -(p1.y * 70) + 500);
		c->setFillColor(Color(255, 0, 0));
		clist.push_back(c);
	}

	//for (int i = 0; i < rectanpoints.size(); i++)
	//{
	//	std::cout << "x: " << rectanpoints[i].x * 100 + 500 << " y: " << -(rectanpoints[i].y * 100) + 500 << std::endl;
	//}

	while (window.isOpen())
	{
		window.clear(Color(100, 100, 100));
		for (int i = 0; i < rectiles.size(); i++)
		{
			rectiles[i].tile->setNum(i);
		}
		for (int i = 0; i < clist.size(); i++)
		{
			window.draw(*clist[i]);
		}
		window.display();
		int b;
		std::cin >> b;
	}
	return 0;
}