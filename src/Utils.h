#ifndef UTILS_H
#define UTILS_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <random>

// Oblicza punkt przecięcia dwóch odcinków. Jest to funkcja matematyczna niezbędna do działania "oczu" (laserów) rakiety – sprawdza, czy promień lasera przecina ścianę przeszkody
inline bool getLineIntersection(sf::Vector2f p1, sf::Vector2f p2, sf::Vector2f p3, sf::Vector2f p4, sf::Vector2f &intersection)
{
    float s1_x = p2.x - p1.x;
    float s1_y = p2.y - p1.y;
    float s2_x = p4.x - p3.x;
    float s2_y = p4.y - p3.y;

    float denom = (-s2_x * s1_y + s1_x * s2_y);
    if (std::abs(denom) < 0.0001f)
        return false;

    float s = (-s1_y * (p1.x - p3.x) + s1_x * (p1.y - p3.y)) / denom;
    float t = (s2_x * (p1.y - p3.y) - s2_y * (p1.x - p3.x)) / denom;

    if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
    {
        intersection.x = p1.x + (t * s1_x);
        intersection.y = p1.y + (t * s1_y);
        return true;
    }
    return false;
}

inline double randomDouble()
{
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_real_distribution<> dis(0.0, 1.0);
    return dis(gen);
}

inline double randomRange(double min, double max)
{
    return min + randomDouble() * (max - min);
}

#endif // UTILS_H
