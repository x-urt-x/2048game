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
const int w_x = 1400, w_y = 1400;
const int spawn_s_q = 4;
const int spawn_q = 2;
const int spawn_r = 3;
const int a = 4;
const int base = 2;
const int max_degree = 99;
//const int r = (w_x / 2 - w_x / 10) / (a * 2 - 1);
const int r = 35;
const float ang = 360.0 / (2 * axis);
int score = 0;
const float Pi = 3.1415;

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

/*
const Color tile_col[] =
{
	Color(10,10,10),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
	Color(12,77,85),
};
*/
RenderWindow window(VideoMode(w_x, w_y), "2048 game");

class Tile : protected sf::CircleShape
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
		else if (num < base * 3)
			text_num.setFillColor(Color(121, 112, 100));
		else
			text_num.setFillColor(Color::White);

		//text_num.setFillColor(Color::White);
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
	Point pointr, pointl;
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

Point roundToPrecision(Point p, float precision)
{
	return Point{ roundf(p.x / precision), roundf(p.y / precision) };
}

bool Tp_compareByX(const Tile_point& a, const Tile_point& b) {
	//return a.point.x < b.point.x;
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
	float bases[8];
	bases[4] = 1;
	bases[5] = 0;
	bases[6] = 0;
	bases[7] = 1;
	std::vector<float*> axistrans;
	for (int k = 0; k < axis; k++)
	{
		bases[0] = cos((k + 1) * ang * Pi / 180);
		bases[1] = sin((k + 1) * ang * Pi / 180);
		bases[2] = cos(k * ang * Pi / 180);
		bases[3] = sin(k * ang * Pi / 180);
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

	int num1 = 4;

	bases[0] = 1;
	bases[1] = 0;
	bases[2] = 0;
	bases[3] = 1;
	for (int k = 0; k < axis; k++)
	{
		bases[4] = cos((k + 1) * ang * Pi/ 180);
		bases[5] = sin((k + 1) * ang * Pi / 180);
		bases[6] = cos(k * ang * Pi / 180);
		bases[7] = sin(k * ang * Pi / 180);
		float* trans = maketrans(bases);
		for (int i = (k == 0 ? 0 : 1); i < points.size(); i++) //points[0] - {0,0} используем только один раз
		{
			Point recpoint = newbase(trans, points[i]); //преобразование в прямоугольные координаты для точек в первой четверти текущего базиса k 
			Tile_point current
			{
				recpoint,points[i],
				new Tile(int(recpoint.x * 100 + w_x / 2 - r), int(-(recpoint.y * 100) + w_y / 2 - r), r, axis * 2)
			};
			for (int j = 0; j < axis; j++) //заполнение одного сектора во всех матрицах
			{
				if (j == k) //пишем в базовую, преобразование координат не нужно
				{
					TP_matrices[j][a - 1 + current.pointl.y].push_back(current);
					continue;
				}
				Point newpoint = newbase(axistrans[j], recpoint); //преобразование из прямоугольных координат в координаты текущего базиса j
				int y;

				if (j == (k - 1 + axis) % axis) //текущая матрица для записи сектора идет следующий после базовой, учитывается только координата х
				{
					y = a - 1 + (k == 0 ? 1 : -1) * current.pointl.x; //при заполнении последний матрицы с первого сектора, в этом случае следующий сектор для базового когда мы будем заполнять последнюю матрицу будет с отрицательными координатами 
				}
				else
				{
					y = a - 1 + (j < k ? -1 : 1) * (current.pointl.x + current.pointl.y);
				}
				current.pointl = newpoint;
				TP_matrices[j][y].push_back(current);
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




	font.loadFromFile("arial.ttf");
	window.setVerticalSyncEnabled(true);

	for (int i = 0; i < spawn_s_q; i++)
	{
		//newTile(matrices[0]);
	}



	for (int i = 0; i < matrices[num1].size(); i++)
	{
		for (int j = 0; j < matrices[num1][i].size(); j++)
		{
			if(matrices[num1][i][j]->getNum() == 0)
				matrices[num1][i][j]->setNum(pow(2, i));
		}
	}

	//matrices[0][0][0]->setNum(4);
	//matrices[0][0][1]->setNum(8);
	//matrices[0][1][0]->setNum(16);

	while (window.isOpen())
	{
		window.clear(Color(100, 100, 100));
		//matrices[0][3][3]->setNum(4);
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
						//newTile(matrices[0]);
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
				if (key >= 0 && key < axis)
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