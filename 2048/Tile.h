#ifndef TILE_H
#define TILE_H

#include <SFML/Graphics.hpp>
#include "StartParams.h"

struct Point
{
    float x, y;
};




class Tile
{
public:
    Tile();
    Tile(Point point);
    Tile(int x, int y);
    void setNum(int numToSet);
    void setPos(int x, int y);
    void setPos(Point point);
    int getNum();
    void upNum();
    void Draw(sf::RenderWindow& window);
    void DrawTexture(sf::RenderTexture& texture);
    static void setStaticParams(sf::Font _font, const StartParamsStruct& StartParams);
private:
    int num = 0;
    sf::Text text_num;
    sf::CircleShape body;
    static sf::Font font;
    static int base, max_degree, r, n;
};

#endif