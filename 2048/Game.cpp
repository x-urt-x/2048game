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
	window.create(sf::VideoMode(StartParams.w_x, StartParams.w_y), "2048 game");
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

	bool moved = 0;

	for (int i = 0; i < StartParams.spawn_s_q; i++)
	{
		newTile(matrices[0]);
	}
	//matrices[0][0][2].tile->setNum(2);
	//matrices[0][0][4].tile->setNum(2);
	//matrices[0][4][2].tile->setNum(2);
	//window.clear(sf::Color(100, 100, 100));
	std::vector<Tile_point*> zeros1;
	Render_GetZeros(zeros1);


	while (window.isOpen())
	{

		window.clear(sf::Color(100, 100, 100));

		std::vector<Tile_point*> zeros;
		Render_GetZeros(zeros);
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
					moved = move(key / 2, key % 2 == 0 ? -1 : 1, 0);
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

//int Game::move(std::vector<std::vector<Tile_point>> matr, int dir, bool v)
//{
//	int moved = 0;
//	std::vector<int> ind;
//	for (int i = 0; i < matr.size(); i++) { ind.push_back(i); }
//	for (int i = 0; ind.size() > 0; i++)
//	{
//		for (int j = 0; j < ind.size(); j++)
//		{
//			int i_w = dir == 1 ? i : matr[ind[j]].size() - i - 1;
//			bool flag_c = false;
//			int num = matr[ind[j]][i_w].tile->getNum();
//			int k_max = matr[ind[j]].size();
//			if (num == 0)
//			{
//				for (int k = i_w + dir; dir == 1 ? k < k_max : k >= 0; k += dir)
//				{
//					if (matr[ind[j]][k].tile->getNum() != 0)
//					{
//						moved = 1;
//						if (!v)
//						{
//							matr[ind[j]][i_w].tile->setNum(matr[ind[j]][k].tile->getNum());
//							matr[ind[j]][k].tile->setNum(0);
//							num = matr[ind[j]][i_w].tile->getNum();
//							break;
//						}
//					}
//					if (((k == k_max - 1) && (dir == 1)) || ((dir == -1) && (k == 0)))
//					{
//						ind.erase(ind.begin() + j);
//						j--;
//						flag_c = true;
//						break;
//					}
//				}
//			}
//			for (int k = i_w + dir; (dir == 1 ? k < k_max : k >= 0) and !flag_c; k += dir)
//			{
//				if (matr[ind[j]][k].tile->getNum() == num)
//				{
//					moved = 1;
//					if (!v)
//					{
//						matr[ind[j]][k].tile->setNum(0);
//						matr[ind[j]][i_w].tile->setNum(num * 2);
//						score += num * 2;
//						break;
//					}
//				}
//				else
//				{
//					if (matr[ind[j]][k].tile->getNum() != 0) break;
//				}
//			}
//			if (((i_w == k_max - 1) && (dir == 1)) || ((dir == -1) && (i_w == 0)))
//			{
//				ind.erase(ind.begin() + j);
//				j--;
//			}
//		}
//	}
//	return moved;
//}


bool Game::move(int matrInd, int dir, bool v)
{

	bool canMove = false;
	std::vector <AnimTile> toAnim; //сюда записываем данные для последующей анимации и изменения элементов
	for (int rowInd = 0; rowInd < matrices[matrInd].size(); rowInd++)
	{
		bool done = true;
		for (int i = (dir > 0 ? 0 : matrices[matrInd][rowInd].size() - 1); (dir < 0 ? i >= 0 : i < matrices[matrInd][rowInd].size()); i += dir) //прямой или обратный перебор в зависимости от dir
		{

			int num = matrices[matrInd][rowInd][i].tile->getNum();
			//если текущий элемент пустой то пытаемся туда что то притянуть
			if (num == 0)
			{
				for (int j = i; (dir < 0 ? j >= 0 : j < matrices[matrInd][rowInd].size()); j += dir)
				{
					if (matrices[matrInd][rowInd][j].tile->getNum() != 0)
					{
						canMove = true; //флаг что сдвиг был
						if (!v)
						{
							done = false;
							num = matrices[matrInd][rowInd][j].tile->getNum();
							matrices[matrInd][rowInd][i].tile->setNum(num);
							toAnim.push_back(AnimTile(matrices[matrInd][rowInd], j, i, dir, num, false, StartParams.stepsPerSec / StartParams.speed));
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
					canMove = true;
					if (!v)
					{
						matrices[matrInd][rowInd][i].tile->setNum(num * 2);
						toAnim.push_back(AnimTile(matrices[matrInd][rowInd], j, i, dir, num, true, StartParams.stepsPerSec / StartParams.speed));
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

	//return canMove;

	//анимация всех строк. я пытался считать каждую строку в отдельном потоке, но классы sfml не поддерживают многопоточность 
	sf::RenderTexture texture;
	sf::Sprite sp;
	sp.setRotation(180);
	texture.create(StartParams.w_x, StartParams.w_y);
	while (toAnim.size() > 0)
	{

		for (int i = 0; i < toAnim.size(); i++)
		{
			toAnim[i].draw(texture);
			if (toAnim[i].move()) //вернет true если движение закончилось
			{
				toAnim.erase(toAnim.begin() + i); //удаляем этот элемент
				i--;
			}
		}
		texture.display();
		sp.setTexture(texture.getTexture());
		window.draw(sp);
		window.display();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1)); //переделать на таймер
	}

	return canMove;
}

//void Game::moveRow(int mi, int ri, int dir, bool v, bool* isEnd, bool* canMove, sf::RenderTexture* texture, bool* stop)
//{
//	bool done = true;
//	std::vector <AnimTile> toAnim; //сюда записываем данные для последующей анимации и изменения элементов
//	for (int i = (dir > 0 ? 0 : matrices[matrInd][rowInd].size() - 1); (dir < 0 ? i >= 0 : i < matrices[matrInd][rowInd].size()); i + dir) //прямой или обратный перебор в зависимости от dir
//	{
//
//		int num = matrices[matrInd][rowInd][i].tile->getNum();
//		//если текущщий элемент пустой то пытаемся туда что то притянуть
//		if (num == 0)
//		{
//			for (int j = i; (dir < 0 ? j >= 0 : j < matrices[matrInd][rowInd].size()); j += dir)
//			{
//				if (matrices[matrInd][rowInd][j].tile->getNum() != 0)
//				{
//					done = false;
//					*canMove = true; //если v==false то это флаг что сдвиг был, если v==true то это только возможность сдвига
//					if (!v)
//					{
//						num = matrices[matrInd][rowInd][j].tile->getNum();
//						toAnim.push_back(AnimTile(matrices[matrInd][rowInd], j, i, dir, num, false, StartParams.stepsPerSec / StartParams.speed));
//						matrices[matrInd][rowInd][j].tile->setNum(0); //элемент который мы притянули сразу ставим 0 
//					}
//					else // при v==true достаточно чтобы хоть один сдвиг был возможен
//					{
//						*isEnd = true;
//						return;
//					}
//				}
//			}
//			//текщий элемент 0 и ничего не притянули значит смещение завершено
//			if (done)
//			{
//				break;
//			}
//		}
//		//если в элементе что то было или мы туда что то притянули то пытаемя притянуть такой же
//		for (int j = i; (dir < 0 ? j >= 0 : j < matrices[matrInd][rowInd].size()); j += dir)
//		{
//			if (matrices[matrInd][rowInd][j].tile->getNum() == num)
//			{
//				*canMove = true;
//				if (!v)
//				{
//					toAnim.push_back(AnimTile(matrices[matrInd][rowInd], j, i, dir, num, true, StartParams.stepsPerSec / StartParams.speed));
//					matrices[matrInd][rowInd][j].tile->setNum(0);
//				}
//				else
//				{
//					*isEnd = true;
//					return;
//				}
//			}
//			else
//			{
//				//первый доступный не такой же как текущий. притягивать можно только через нули
//				if (matrices[matrInd][rowInd][j].tile->getNum() != 0) break;
//			}
//		}
//		done = true;
//	}
//
//	if (v) return; //анимация не нужна
//
//
//
//	while (!toAnim.empty())
//	{
//		for (int i = 0; i < toAnim.size(); i++)
//		{
//			toAnim[i].draw(texture);
//			if (toAnim[i].move()) //вернет true если движение закончилось
//			{
//				toAnim.erase(toAnim.begin() + i); //удаляем этот элемент
//				i--;
//			}
//		}
//		done = done;
//		while (*stop)
//		{
//			std::this_thread::sleep_for(std::chrono::milliseconds(100 / StartParams.stepsPerSec));
//		}
//	}
//	*isEnd = true;
//}


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
			recpoint.x = int(recpoint.x * StartParams.range + StartParams.w_x / 2 - StartParams.r);
			recpoint.y = int(-(recpoint.y * StartParams.range) + StartParams.w_y / 2 - StartParams.r);
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
	//int n = 3;
	//for (int i = 0; i < matrices[n].size(); i++)
	//{
	//	for (int j = 0; j < matrices[n][i].size(); j++)
	//	{
	//		matrices[n][i][j].tile->setNum(pow(2, j));
	//	}
	//}


	return matrices;
}

