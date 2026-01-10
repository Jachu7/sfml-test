#ifndef LASER_READING_H
#define LASER_READING_H

#include <SFML/Graphics.hpp>

struct LaserReading
{
    sf::Vector2f endPoint;
    float distance;
    bool hit;
};

#endif // LASER_READING_H
