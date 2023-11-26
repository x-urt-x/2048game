#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "StartParams.h"

class Tile
{
public:
    Tile();
    Tile(int x, int y);
    void setNum(int numToSet);
    int getNum();
    void upNum();
    void Draw(sf::RenderWindow& window);
    static void setStaticParams(sf::Font _font, const StartParamsStruct& StartParams);

private:
    int num = 0;
    sf::Text text_num;
    sf::CircleShape body;
    static sf::Font font;
    static int base, max_degree, r, n;
};

#endif