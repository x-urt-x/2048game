#include "Game.h"

bool Tp_compare(const Tile_point& a, const Tile_point& b)
{
	if ((a.pointl.x > b.pointl.x - 0.001 && a.pointl.x < b.pointl.x + 0.001)) //проверка на равенство с учетом погрешностей (лежание в окрестности точки)
	{
		return a.pointl.y < b.pointl.y; //в случае если по x координаты равны, учитаваем y 
	}
	else
	{
		return a.pointl.x < b.pointl.x;
	}
}

Game::Game(StartParamsStruct& _StartParams) : StartParams(_StartParams)
{
	window.create(sf::VideoMode(1400, 1400), "2048 game");
	window.setVerticalSyncEnabled(true);
	font.loadFromFile("arial.ttf");
	scores.setFont(font);
	msg.setFont(font);
	scores.setPosition(StartParams.w_x / 2 - StartParams.w_x / 10, StartParams.w_y / 20);
	msg.setPosition(StartParams.w_x / 20, StartParams.w_y / 2 - StartParams.w_y / 10);
	scores.setCharacterSize(StartParams.w_y / 20);
	msg.setCharacterSize(StartParams.w_y / 5);
	msg.setFillColor(sf::Color::Red);
	srand(time(NULL));
	state = 0;
	Tile::setStaticParams(font, StartParams);
	matrices = CreateMatrices();

}


void Game::Render_GetZeros(std::vector<Tile_point*>& zeros)
{
		for (int i = 0; i < matrices[0].size(); i++)
		{
			for (int j = 0; j < matrices[0][i].size(); j++)
			{
				int num = matrices[0][i][j].tile->getNum();
				if (num == 0)
				{
					zeros.push_back(&(matrices[0][i][j]));
				}
				else if (log(num) / log(2) >= StartParams.max_degree && state != 2) state = 2;
				matrices[0][i][j].tile->upNum();
				matrices[0][i][j].tile->Draw(window);
			}
		}
		window.draw(scores);
}



void Game::Run()
{

	int moved=0;

	for (int i = 0; i < StartParams.spawn_s_q; i++)
	{
		newTile(matrices[0]);
	}
	while (window.isOpen())
	{
		window.clear(sf::Color(100, 100, 100));

		std::vector<Tile_point*> zeros;
		Render_GetZeros(zeros);
		if (moved != 0)
		{
			for (int i = 0; i < StartParams.spawn_q && i < zeros.size(); i++)
			{
				newTile(matrices[0]);
			}
			if (StartParams.spawn_q > zeros.size())
				state = 3;

			if (state == 3)
			{
				state = 0;
				bool l1 = move(matrices[0], 1, 1) || move(matrices[0], -1, 1);
				for (int k = 1; k < StartParams.axis; k++)
					l1 = l1 || move(matrices[k], 1, 1) || move(matrices[k], -1, 1);
				if (!l1)
					state = 3;
			}

			if (state == 2)
			{
				msg.setString((sf::String)("you win"));
				window.draw(msg);
				window.display();
				Sleep(5000);
				msg.setString((sf::String)(""));
			}
			if (state == 3)
			{
				msg.setString((sf::String)("you lose"));
				window.draw(msg);
				window.display();
				Sleep(5000);
				msg.setString((sf::String)(""));
			}

			scores.setString(std::to_string(score));
			moved = 0;
		}

		sf::Event event;
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
				if (key >= 0 && key < StartParams.axis * 2)
					moved = move(matrices[key / 2], key % 2 == 0 ? -1 : 1, 0);
			}
			break;
			default:
				break;
			}
		}
		window.display();
	}
}

void Game::newTile(std::vector<std::vector<Tile_point>> matr)
{
	int r_x, r_y;
	do
	{
		r_x = rand() % matr.size();
		r_y = rand() % matr[r_x].size();
	} while (matr[r_x][r_y].tile->getNum() != 0);
	matr[r_x][r_y].tile->setNum(StartParams.base * pow(2, (rand()) % StartParams.spawn_r));
}

float* Game::maketrans(float bases[])
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

Point Game::newbase(float trans[], Point oldpoint)
{
	Point newpoint;
	newpoint.x = trans[0] * oldpoint.x + trans[2] * oldpoint.y;
	newpoint.y = trans[1] * oldpoint.x + trans[3] * oldpoint.y;
	return newpoint;
}

int Game::move(std::vector<std::vector<Tile_point>> matr, int dir, bool v)
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
			int num = matr[ind[j]][i_w].tile->getNum();
			int k_max = matr[ind[j]].size();
			if (num == 0)
			{
				for (int k = i_w + dir; dir == 1 ? k < k_max : k >= 0; k += dir)
				{
					if (matr[ind[j]][k].tile->getNum() != 0)
					{
						moved = 1;
						if (!v)
						{
							matr[ind[j]][i_w].tile->setNum(matr[ind[j]][k].tile->getNum());
							matr[ind[j]][k].tile->setNum(0);
							num = matr[ind[j]][i_w].tile->getNum();
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
				if (matr[ind[j]][k].tile->getNum() == num)
				{
					moved = 1;
					if (!v)
					{
						matr[ind[j]][k].tile->setNum(0);
						matr[ind[j]][i_w].tile->setNum(num * 2);
						score += num * 2;
						break;
					}
				}
				else
				{
					if (matr[ind[j]][k].tile->getNum() != 0) break;
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

std::vector<std::vector<std::vector<Tile_point>>> Game::CreateMatrices()
{
	const float ang = 360.0 / (2 * StartParams.axis);
	const float Pi = 3.1415926535;
	std::vector<Point> points;
	points.push_back(Point{ 0,0 });
	for (int i = 1; i < StartParams.a; i++)
	{
		for (int j = 0; j < StartParams.a - i; j++)
		{
			Point p = { i,j };
			points.push_back(p);
			Point p1 = { -i,-j };
			points.push_back(p1);
		}
	}
	float bases[8];
	std::vector<float*> axistrans;
	for (int k = 0; k < StartParams.axis; k++)
	{
		bases[4] = 1;
		bases[5] = 0;
		bases[6] = 0;
		bases[7] = 1;
		bases[0] = cos((k + 1) * ang * Pi / 180);
		bases[1] = sin((k + 1) * ang * Pi / 180);
		bases[2] = cos(k * ang * Pi / 180);
		bases[3] = sin(k * ang * Pi / 180);
		axistrans.push_back(maketrans(bases));
	}

	std::vector<std::vector<std::vector<Tile_point>>> matrices;
	for (int k = 0; k < StartParams.axis; k++)
	{
		std::vector<std::vector<Tile_point>> matr;
		for (int i = 0; i < 2 * StartParams.a - 1; i++) //колличесво рядов паралельных сдвигов равно диаметру -----------------------------------------------
		{
			std::vector<Tile_point> row;
			matr.push_back(row);
		}
		matrices.push_back(matr);
	}



	for (int k = 0; k < StartParams.axis; k++)
	{
		bases[0] = 1;
		bases[1] = 0;
		bases[2] = 0;
		bases[3] = 1;
		bases[4] = cos((k + 1) * ang * Pi / 180);
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
				new Tile(int(recpoint.x * 100 + StartParams.w_x / 2 - StartParams.r), int(-(recpoint.y * 100) + StartParams.w_y / 2 - StartParams.r))  // исправть ---------------------------------------------
			};
			for (int j = 0; j < StartParams.axis; j++) //заполнение одного сектора во всех матрицах
			{
				if (j == k) //пишем в базовую, преобразование координат не нужно
				{
					matrices[j][StartParams.a - 1 + current.pointl.y].push_back(current);
					continue;
				}
				Point newpoint = newbase(axistrans[j], recpoint); //преобразование из прямоугольных координат в координаты текущего базиса j
				int y;

				if (j == (k - 1 + StartParams.axis) % StartParams.axis) //текущая матрица для записи сектора идет следующий после базовой, учитывается только координата х
				{
					y = StartParams.a - 1 + (k == 0 ? 1 : -1) * current.pointl.x; //при заполнении последний матрицы с первого сектора, в этом случае следующий сектор для базового когда мы будем заполнять последнюю матрицу будет с отрицательными координатами 
				}
				else
				{
					y = StartParams.a - 1 + (j < k ? -1 : 1) * (current.pointl.x + current.pointl.y);
				}
				matrices[j][y].push_back(Tile_point{ current.pointr,newpoint,current.tile });
			}
		}
	}
	for (int k = 0; k < StartParams.axis; k++)
	{
		for (int i = 0; i < 2 * StartParams.a - 1; i++)
		{
			std::sort(matrices[k][i].begin(), matrices[k][i].end(), Tp_compare);
		}
	}
	return matrices;
}

