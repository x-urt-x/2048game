#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include <math.h>
#include <time.h>
#include "windows.h"


using namespace sf;


Font font;
const float prec = 0.05;
const int axis = 5;
const int w_x = 1200, w_y = 1200;
const int spawn_s_q = 4;
const int spawn_q = 2;
const int spawn_r = 3;
const int a = 4;
const int base = 2;
const int max_degree = 16;
//const int r = (w_x / 2 - w_x / 10) / (a * 2 - 1);
const int r = 25;
const float ang = 360.0 / (2 * axis);
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
	Tile(int x, int y, int r, int n)

		:CircleShape(r, n)
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

void newTile(std::vector < std::vector<Tile*>>& matr)
{
	int r_x, r_y;
	do
	{
		r_x = rand() % matr.size();
		r_y = rand() % matr[r_x].size();
	} while (matr[r_x][r_y]->getNum() != 0);
	matr[r_x][r_y]->setNum(base * pow(2, (rand()) % spawn_r));
}


int move(std::vector < std::vector<Tile*>>& matr, int dir, bool v)
{
	int moved = 0;
	std::vector<int> ind;
	for (int i = 0; i < matr.size(); i++) { ind.push_back(i); }
	for (int i = 0; ind.size() > 0; i++)
	{
		for (int j = 0; j < ind.size(); j++)
		{
			int i_w = dir == 1 ? i : matr[ind[j]].size() - i - 1;
			bool flag_c = false;
			int num = matr[ind[j]][i_w]->getNum();
			int k_max = matr[ind[j]].size();
			if (num == 0)
			{
				for (int k = i_w + dir; dir == 1 ? k < k_max : k >= 0; k += dir)
				{
					if (matr[ind[j]][k]->getNum() != 0)
					{
						moved = 1;
						if (!v)
						{
							matr[ind[j]][i_w]->setNum(matr[ind[j]][k]->getNum());
							matr[ind[j]][k]->setNum(0);
							num = matr[ind[j]][i_w]->getNum();
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
				if (matr[ind[j]][k]->getNum() == num)
				{
					moved = 1;
					if (!v)
					{
						matr[ind[j]][k]->setNum(0);
						matr[ind[j]][i_w]->setNum(num * 2);
						score += num * 2;
						break;
					}
				}
				else
				{
					if (matr[ind[j]][k]->getNum() != 0) break;
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

float* maketrans(float bases[])
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

	bases[2] /= bases[0];
	bases[4] /= bases[0];
	bases[6] /= bases[0];
	if (bases[0] != 0)
	{
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

Point newbase(float trans[], Point oldpoint)
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

Point roundToPrecision(Point p, float precision)
{
	return Point{ roundf(p.x / precision), roundf(p.y / precision) };
}

bool Tp_compareByX(const Tile_point& a, const Tile_point& b) {
	return a.point.x < b.point.x;
}

bool Tp_compareByY(const Tile_point& a, const Tile_point& b) {
	return a.point.y < b.point.y;
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

	//std::vector<Point> points, recpoints;
	//for (int i = 0; i < a; i++)
	//{
	//	for (int j = 1; j < a - i; j++)
	//	{
	//		Point p = { i,j };
	//		points.push_back(p);
	//		Point p1 = { -i,-j };
	//		points.push_back(p1);
	//	}
	//}

	float bases[8];
	//bases[0] = 1;
	//bases[1] = 0;
	//bases[2] = 0;
	//bases[3] = 1;
	//for (int i = 0; i < axis; i++)
	//{
	//	//bases[4] = cos((i + 1.5) * ang * 3.14 / 180);
	//	//bases[5] = sin((i + 1.5) * ang * 3.14 / 180);
	//	//bases[6] = cos((i + 0.5) * ang * 3.14 / 180);
	//	//bases[7] = sin((i + 0.5) * ang * 3.14 / 180);
	//	bases[4] = cos((i + 1) * ang * 3.14 / 180);
	//	bases[5] = sin((i + 1) * ang * 3.14 / 180);
	//	bases[6] = cos(i * ang * 3.14 / 180);
	//	bases[7] = sin(i * ang * 3.14 / 180);
	//	addnewpoints(&recpoints, points, bases);
	//}


	std::vector<Point> points;
	points.push_back(Point{ 0,0 });
	for (int i = 1; i < a; i++)
	{
		for (int j = 0; j < a - i; j++)
		{
			Point p = { i,j };
			points.push_back(p);
			Point p1 = { -i,-j };
			points.push_back(p1);
		}
	}
	//float bases[8];
	bases[4] = 1;
	bases[5] = 0;
	bases[6] = 0;
	bases[7] = 1;
	std::vector<float*> axistrans;
	for (int k = 0; k < axis; k++)
	{
		bases[0] = cos((k + 1) * ang * 3.14 / 180);
		bases[1] = sin((k + 1) * ang * 3.14 / 180);
		bases[2] = cos(k * ang * 3.14 / 180);
		bases[3] = sin(k * ang * 3.14 / 180);
		axistrans.push_back(maketrans(bases));
	}

	std::vector<std::vector<std::vector<Tile_point>>> TP_matrices;
	for (int k = 0; k < axis; k++)
	{
		std::vector<std::vector<Tile_point>> TP_matr;
		for (int i = 0; i < 2 * a - 1; i++) //колличесво рядов паралельных сдвигов равно диаметру
		{
			std::vector<Tile_point> TP_row;
			TP_matr.push_back(TP_row);
		}
		TP_matrices.push_back(TP_matr);
	}

	bases[0] = 1;
	bases[1] = 0;
	bases[2] = 0;
	bases[3] = 1;
	for (int k = 0; k < axis; k++)
	{
		bases[4] = cos((k + 1) * ang * 3.14 / 180);
		bases[5] = sin((k + 1) * ang * 3.14 / 180);
		bases[6] = cos(k * ang * 3.14 / 180);
		bases[7] = sin(k * ang * 3.14 / 180);
		float* trans = maketrans(bases);
		for (int i = (k == 0 ? 0 : 1); i < points.size(); i++)
		{
			Point recpoint = newbase(trans, points[i]);
			Tile_point current
			{
				points[i],
				new Tile(int(recpoint.x * 70 + w_x / 2 - r), int(-(recpoint.y * 70) + w_y / 2 - r), r, axis * 2)
			};
			for (int j = 0; j < axis; j++) //заполнение одного сектора во всех матрицах
			{
				if (j == k)
				{
					TP_matrices[j][a - 1 + current.point.y].push_back(current);
					continue;
				}
				Point newpoint = newbase(axistrans[j], recpoint);
				TP_matrices[j][a - 1 + current.point.x].push_back(Tile_point{ newpoint, current.tile });
			}
		}
	}


	std::vector<std::vector<std::vector<Tile*>>> matrices;
	for (int k = 0; k < axis; k++)
	{
		std::vector<std::vector<Tile*>> matr;
		for (int i = 0; i < 2 * a - 1; i++)
		{
			std::vector<Tile*> row;
			std::sort(TP_matrices[k][i].begin(), TP_matrices[k][i].end(), Tp_compareByX);
			for (int j = 0; j < TP_matrices[k][i].size(); j++)
			{
				row.push_back(TP_matrices[k][i][j].tile);
			}
			matr.push_back(row);
		}
		matrices.push_back(matr);
	}









	//std::vector<Tile_point> rectiles;
	//rectiles.push_back(Tile_point{ Point{0,0},new Tile(int(w_x / 2 - r), int(w_y / 2 - r), r, axis * 2) }); //центральное поле
	//for (int i = 0; i < recpoints.size(); i++)
	//{
	//	rectiles.push_back(
	//		Tile_point
	//		{
	//			Point{recpoints[i].x,recpoints[i].y},
	//			new Tile(int(recpoints[i].x * 70 + w_x / 2 - r), int(-(recpoints[i].y * 70) + w_y / 2 - r), r, axis * 2)
	//		}
	//	);
	//}

	//std::vector<std::vector<Tile_point>> axislists;
	//for (int k = 0; k < axis; k++)
	//{
	//	//bases[0] = cos((k + 1.5) * ang * 3.14 / 180);
	//	//bases[1] = sin((k + 1.5) * ang * 3.14 / 180);
	//	//bases[2] = cos((k + 0.5) * ang * 3.14 / 180);
	//	//bases[3] = sin((k + 0.5) * ang * 3.14 / 180);
	//	bases[0] = cos((k + 1) * ang * 3.14 / 180);
	//	bases[1] = sin((k + 1) * ang * 3.14 / 180);
	//	bases[2] = cos(k * ang * 3.14 / 180);
	//	bases[3] = sin(k * ang * 3.14 / 180);
	//	bases[4] = 1;
	//	bases[5] = 0;
	//	bases[6] = 0;
	//	bases[7] = 1;
	//	float* trans = maketrans(bases);
	//	std::vector<Tile_point> tiles;
	//	for (int i = 0; i < rectiles.size(); i++)
	//	{
	//		tiles.push_back(
	//			Tile_point
	//			{
	//				roundToPrecision(newbase(trans, rectiles[i].point),prec),
	//				//newbase(trans, rectiles[i].point),
	//				rectiles[i].tile
	//			}
	//		);
	//	}
	//	axislists.push_back(tiles);
	//}
	//std::vector<std::vector<std::vector<Tile*>>> matrices;
	//for (int k = 0; k < axis; k++)
	//{
	//	std::vector<std::vector<Tile*>> matr;
	//	std::sort(axislists[k].begin(), axislists[k].end(), Tp_compareByY);
	//	for (int i = 0; i < axislists[k].size();)
	//	{
	//		std::vector<Tile_point> Tp_row;
	//		float y = axislists[k][i].point.y;
	//		while (i < axislists[k].size() && y == axislists[k][i].point.y)
	//		{
	//			Tp_row.push_back(axislists[k][i]);
	//			i++;
	//		}
	//		std::sort(Tp_row.begin(), Tp_row.end(), Tp_compareByX);
	//		std::vector<Tile*> row;
	//		for (int j = 0; j < Tp_row.size(); j++)
	//		{
	//			row.push_back(Tp_row[j].tile);
	//		}
	//		matr.push_back(row);
	//	}
	//	matrices.push_back(matr);
	//}





	//bases[0] = 1;
	//bases[1] = 0;
	//bases[2] = 0;
	//bases[3] = 1;

	//int k = 1;
	//bases[4] = cos((k + 1) * ang * 3.14 / 180);
	//bases[5] = sin((k + 1) * ang * 3.14 / 180);
	//bases[6] = cos(k * ang * 3.14 / 180);
	//bases[7] = sin(k * ang * 3.14 / 180);
	//float* trans = maketrans(bases);
	//std::vector<CircleShape*> clist;
	//for (int i = 0; i < axislists[0].size(); i++)
	//{
	//	Point p1 = newbase(trans, axislists[k][i].point);
	//	CircleShape* c = new sf::CircleShape(25);
	//	c->setPosition(p1.x * 70 + 500, -(p1.y * 70) + 500);
	//	c->setFillColor(Color(255, 0, 0));
	//	clist.push_back(c);
	//}


	font.loadFromFile("arial.ttf");
	window.setVerticalSyncEnabled(true);

	for (int i = 0; i < spawn_s_q; i++)
	{
//		newTile(matrices[0]);
	}

	while (window.isOpen())
	{
		window.clear(Color(100, 100, 100));
		for (int i = 0; i < matrices[0].size(); i++)
		{
			for (int j = 0; j < matrices[0][i].size(); j++)
			{
				num = matrices[0][i][j]->getNum();
				if (num == 0)
				{
					lose = false;
					if (moved != 0 && moved <= spawn_q)
					{
						newTile(matrices[0]);
						moved++;
					}
				}
				else if (log(num) / log(2) >= max_degree && won == 0) won = 2;
				matrices[0][i][j]->upNum();
			}
		}
		if (lose)
		{

			bool l1 = move(matrices[0], 1, 1) || move(matrices[0], -1, 1);
			for (int k = 1; k < axis; k++)
				l1 = l1 || move(matrices[k], 1, 1) || move(matrices[k], -1, 1);
			if (!l1)
			{
				stop = true;
				msg.setString(static_cast<String>("you lose"));
			}
		}
		if (won == 2)
		{
			msg.setString(static_cast<String>("you win"));
			window.draw(msg);
			window.display();
			Sleep(5000);
			msg.setString(static_cast<String>(""));
			won = 1;

		}
		scores.setString(std::to_string(score));
		window.draw(msg);
		window.draw(scores);



		Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				// закрытие окна
			case sf::Event::Closed:
				window.close();
				break;
				// нажатие клавиши

			case sf::Event::KeyPressed:
				//if (!stop)
			{
				if (event.key.code == 78)
				{
					newTile(matrices[0]);
					break;
				}
				int key = event.key.code - 26;
				moved = move(matrices[key / 2], key % 2 == 0 ? -1 : 1, 0);
			}
			break;
			default:
				break;
			}
		}
		window.display();
		lose = true;
	}
	return 0;
}