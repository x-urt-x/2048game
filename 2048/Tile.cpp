#include "Tile.h"

const sf::Color tile_col[] =
{
    sf::Color(97, 77, 60),
    sf::Color(250, 231, 224),
    sf::Color(249, 225, 206),
    sf::Color(249, 179, 124),
    sf::Color(246, 149, 96),
    sf::Color(243, 125, 95),
    sf::Color(254, 92, 65),
    sf::Color(245, 208, 119),
    sf::Color(249, 208, 103),
    sf::Color(249, 202, 88),
    sf::Color(239, 196, 42),
    sf::Color(244, 198, 42)
};


Tile::Tile()
{
}

Tile::Tile(Point point) :Tile(point.x, point.y)
{
}

Tile::Tile(int x, int y)
{
    body.setPointCount(n);
    body.setRadius(r);
    text_num.setCharacterSize(r / 2);
    text_num.setFont(font);
    setPos(x, y);
    num = 0;
    upNum();
}

void Tile::setNum(int numToSet)
{
    num = numToSet;
    upNum();
}

void Tile::setPos(int x, int y)
{
    body.setPosition(x, y);
    text_num.setPosition(x + r * 0.866 - r / 4, y + r / 2);
}

void Tile::setPos(Point point)
{
    setPos(point.x, point.y);
}

int Tile::getNum()
{
    return num;
}

void Tile::upNum()
{
    text_num.setString(std::to_string(num));

    if (num == 0)
        text_num.setFillColor(tile_col[0]);
    else if (num < base * 3)
        text_num.setFillColor(sf::Color(121, 112, 100));
    else
        text_num.setFillColor(sf::Color::White);

    if (log(num) / log(base) <= std::size(tile_col)-1)
        body.setFillColor(tile_col[int(num == 0 ? 0 : log(num) / log(base))]);
    else
        body.setFillColor(tile_col[std::size(tile_col)-1]);
}

void Tile::Draw(sf::RenderWindow& window)
{
    window.draw(body);
    window.draw(text_num);
}

void Tile::DrawTexture(sf::RenderTexture& texture)
{
    texture.draw(body);
    texture.draw(text_num);
}

void Tile::setStaticParams(sf::Font _font, const StartParamsStruct& StartParams)
{
    font = _font;
    base = StartParams.base;
    max_degree = StartParams.max_degree;
    r = StartParams.r;
    n = StartParams.axis*2;
}

sf::Font Tile::font;
int Tile::base;
int Tile::max_degree;
int Tile::r;
int Tile::n;
