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

void setArrowPoints(sf::ConvexShape& arrow, int arrow_width)
{
	arrow.setPoint(0, sf::Vector2f(0, -0.06 * arrow_width));
	arrow.setPoint(1, sf::Vector2f(0.6 * arrow_width, -0.06 * arrow_width));
	arrow.setPoint(2, sf::Vector2f(0.6 * arrow_width, -0.2 * arrow_width));
	arrow.setPoint(3, sf::Vector2f(arrow_width, 0));
	arrow.setPoint(4, sf::Vector2f(0.6 * arrow_width, 0.2 * arrow_width));
	arrow.setPoint(5, sf::Vector2f(0.6 * arrow_width, 0.06 * arrow_width));
	arrow.setPoint(6, sf::Vector2f(0, 0.06 * arrow_width));
}

Game::Game(StartParamsStruct& _StartParams) : StartParams(_StartParams)
{
	window.create(sf::VideoMode(StartParams.w_x, StartParams.w_y), "2048 game");
	window.setVerticalSyncEnabled(true);
	font.loadFromFile("arial.ttf");
	scores.setFont(font);
	msg.setFont(font);
	scores.setPosition(StartParams.w_x * 0.025, StartParams.w_y * 0.05);
	msg.setPosition(StartParams.w_x / 20, StartParams.w_y / 2 - StartParams.w_y / 10);
	scores.setCharacterSize(StartParams.w_y / 20);
	scores.setString("0");
	msg.setCharacterSize(StartParams.w_y / 5);
	msg.setFillColor(sf::Color::Red);
	msg.setString((sf::String)(""));
	srand(time(NULL));
	state = 0;
	Tile::setStaticParams(font, StartParams);
	matrices = CreateMatrices();
	for (int i = 1; i <= StartParams.axis; i++)
	{
		{
			sf::ConvexShape arrow(7);
			setArrowPoints(arrow, StartParams.field.left * 0.5);
			arrow.setRotation(-180.0 / StartParams.axis * i);
			arrow.setPosition(
				 StartParams.range * (StartParams.a - 0.5) * cos(3.14 / StartParams.axis * i) + StartParams.field.width / 2 + StartParams.field.left,
				-StartParams.range * (StartParams.a - 0.5) * sin(3.14 / StartParams.axis * i) + StartParams.field.height / 2 + StartParams.field.top
			);
			arrows.push_back(arrow);
		}
		sf::ConvexShape arrow(7);
		setArrowPoints(arrow, StartParams.field.left * 0.5);
		arrow.setRotation(180 - 180.0 / StartParams.axis * i);
		arrow.setPosition(
			-StartParams.range * (StartParams.a - 0.5) * cos(3.14 / StartParams.axis * i) + StartParams.field.width / 2 + StartParams.field.left,
			StartParams.range * (StartParams.a - 0.5) * sin(3.14 / StartParams.axis * i) + StartParams.field.height / 2 + StartParams.field.top
		);
		arrows.push_back(arrow);
	}

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
	for (int i = 0; i < arrows.size(); i++)
	{
		window.draw(arrows[i]);
	}
}

void Game::Run()
{
	//sf::Vector2i cursorPos;
	bool moved = false;
	sf::Event event;

	for (int i = 0; i < StartParams.spawn_s_q; i++)
	{
		newTile(matrices[0]);
	}

	window.clear(sf::Color(100, 100, 100));
	std::vector<Tile_point*> zeros1;
	Render_GetZeros(zeros1);


	while (window.isOpen())
	{
		window.clear(sf::Color(100, 100, 100));
		std::vector<Tile_point*> zeros;
		Render_GetZeros(zeros);

		sf::Time();
		window.draw(msg);
		if (moved)
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
				bool l1 = move(0, 1, 1) || move(0, -1, 1);
				for (int k = 1; k < StartParams.axis; k++)
					l1 = l1 || move(k, 1, 1) || move(k, -1, 1);
				if (!l1)
					state = 3;
			}

			if (state == 2)
			{
				msg.setString((sf::String)("You win"));
			}
			if (state == 3)
			{
				msg.setString((sf::String)("Game over!"));
			}

			scores.setString(std::to_string(score));
			moved = false;

		}


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
			{
				if (event.key.code == 78)
				{
					newTile(matrices[0]);
					break;
				}
				int key = event.key.code - 26;
				if (key >= 0 && key < StartParams.axis * 2)
					moved = move(key / 2, key % 2 == 0 ? -1 : 1, 0);
			}
			break;
			case sf::Event::MouseButtonPressed:
			{
				for (int i = 0; i < arrows.size(); i++)
				{
					sf::FloatRect arrow = arrows[i].getGlobalBounds();
					sf::Vector2i mouse = sf::Mouse::getPosition(window);
					if (mouse.x > arrow.left && mouse.x < arrow.left + arrow.width && mouse.y > arrow.top && mouse.y < arrow.top + arrow.height)
						moved = move(i / 2, i % 2 == 0 ? -1 : 1, 0);
				}
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

bool Game::move(int matrInd, int dir, bool v)
{
	bool moved = false;
	for (int rowInd = 0; rowInd < matrices[matrInd].size(); rowInd++)
	{
		for (int i = (dir > 0 ? 0 : matrices[matrInd][rowInd].size() - 1); (dir < 0 ? i >= 0 : i < matrices[matrInd][rowInd].size()); i += dir) //прямой или обратный перебор в зависимости от dir
		{
			bool done = false;
			int num = matrices[matrInd][rowInd][i].tile->getNum();
			//если текущий элемент пустой то пытаемся туда что то притянуть
			if (num == 0)
			{
				done = true;
				for (int j = i; (dir < 0 ? j >= 0 : j < matrices[matrInd][rowInd].size()); j += dir)
				{
					if (matrices[matrInd][rowInd][j].tile->getNum() != 0)
					{
						moved = true; //флаг что сдвиг был
						if (!v)
						{
							done = false;
							num = matrices[matrInd][rowInd][j].tile->getNum();
							matrices[matrInd][rowInd][i].tile->setNum(num);
							matrices[matrInd][rowInd][j].tile->setNum(0); //элемент который мы притянули сразу ставим 0 
							break;
						}
						else // при v==true достаточно чтобы хоть один сдвиг был возможен
						{
							return true;
						}
					}
				}
				//текщий элемент 0 и ничего не притянули значит смещение завершено
				if (done)
				{
					break;
				}
			}
			//если в элементе что то было или мы туда что то притянули то пытаемя притянуть такой же
			for (int j = i + dir; (dir < 0 ? j >= 0 : j < matrices[matrInd][rowInd].size()) && !done; j += dir)
			{
				if (matrices[matrInd][rowInd][j].tile->getNum() == num)
				{
					moved = true;
					if (!v)
					{
						matrices[matrInd][rowInd][i].tile->setNum(num * 2);
						matrices[matrInd][rowInd][j].tile->setNum(0);
						score += num * 2;
						break;
					}
					else
					{
						return true;
					}
				}
				else
				{
					//первый доступный не такой же как текущий. притягивать можно только через нули
					if (matrices[matrInd][rowInd][j].tile->getNum() != 0) break;
				}
			}
			done = true;
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

	//создание шаблона для одного сектора
	//координаты точек только науральные числа.
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

	//расчет колличества рядов при произвольной конфигурации шаблона сектора. исправить --------------------------
	//int rows = 0;
	//for (int i = 0; i < std::size(points); i++)
	//{
	//	if (points[i].y + points[i].x > rows)
	//		rows = points[i].y + points[i].x;
	//}
	//rows *= 2;
	//rows += 1;


	//создание матрицы перехода из прямоугольного базиса в проивольный
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

	//самый главный вектор. в нем будут все матрицы сдвига
	std::vector<std::vector<std::vector<Tile_point>>> matrices;
	for (int k = 0; k < StartParams.axis; k++)
	{
		std::vector<std::vector<Tile_point>> matr;
		//for (int i = 0; i < rows; i++)
		for (int i = 0; i < 2 * StartParams.a - 1; i++) //колличесво рядов паралельных сдвигов равно диаметру
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
			//преобразование в прямоугольные координаты для точек в первой четверти текущего базиса k
			Point recpoint = newbase(trans, points[i]);
			Point recpoint1 = recpoint;
			recpoint.x = int(recpoint.x * StartParams.range + StartParams.field.width / 2 + StartParams.field.left - StartParams.r);
			recpoint.y = int(-(recpoint.y * StartParams.range) + StartParams.field.height / 2 + StartParams.field.top - StartParams.r);
			Tile_point current
			{
				recpoint, points[i],
				new Tile(recpoint)
			};

			//заполнение одного сектора во всех матрицах
			for (int j = 0; j < StartParams.axis; j++)
			{
				if (j == k) //пишем в базовую, преобразование координат не нужно
				{
					matrices[j][StartParams.a - 1 + points[i].y].push_back(current);
				}
				else
				{
					current.pointl = newbase(axistrans[j], recpoint1); //преобразование из прямоугольных координат в координаты текущего базиса j

					//вычисляем ряд
					int row;
					if (j == (k - 1 + StartParams.axis) % StartParams.axis) //текущая матрица для записи сектора идет следующий после базовой, учитывается только координата х
					{
						row = StartParams.a - 1 + (k == 0 ? 1 : -1) * points[i].x; //при заполнении последний матрицы с первого сектора, в этом случае следующий сектор для базового когда мы будем заполнять последнюю матрицу будет с отрицательными координатами 
					}
					else
					{
						row = StartParams.a - 1 + (j < k ? -1 : 1) * (points[i].x + points[i].y);
					}

					matrices[j][row].push_back(current);
				}
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

	//проверка матрицы
	//int n = 1;
	//for (int i = 0; i < matrices[n].size(); i++)
	//{
	//	for (int j = 0; j < matrices[n][i].size(); j++)
	//	{
	//		matrices[n][i][j].tile->setNum(pow(2, j));
	//	}
	//}


	return matrices;
}


