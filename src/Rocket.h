#ifndef ROCKET_H
#define ROCKET_H

#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <algorithm>

#include "siec/Neuron.h"
#include "siec/Matrix.h"
#include "siec/NeuralNetwork.h"
#include "siec/utils/MultiplyMatrix.h"

#include "LaserReading.h"
#include "Utils.h"

// stała do obliczen
const double M_PI_VAL = 3.14159265358979323846;

// struktura rakiety
struct Rocket
{
    // konfiguracja
    sf::Vector2f velocity = {0.f, 0.f};
    sf::Sprite sprite;
    sf::Sprite fireSprite;
    std::vector<LaserReading> lasers;

    // stan
    NeuralNetwork *brain = nullptr;
    double fitness = 0.0;
    bool dead = false;
    bool completed = false;
    bool isThrusting = false;

    // statystyki
    int timeAlive = 0;
    std::vector<bool> visitedCheckpoints;
    float bestDistanceToTarget = 999999.f; // Resetowane przy każdym checkpoincie
    sf::Vector2f lastPosition = {0.f, 0.f};
    int stuckCounter = 0;

    // parametry fizyczne
    const float gravity = 0.02f;
    const float thrustPower = 0.1f;
    const float rotationSpeed = 3.0f;
    const float maxLaserDist = 400.0f;
    std::vector<float> laserAngles = {-90.f, -45.f, -20.f, 0.f, 20.f, 45.f, 90.f, 180.f};

    // topologia sieci neuronowej, 13 wejsc, 8 warstw ukrytych i 3 wyjscia
    const std::vector<int> topology = {13, 8, 3};

    // skalowanie raycastów (laserów) oraz tekstur rakiety (i inicjacja sieci)
    Rocket(const sf::Texture &shipTexture, const sf::Texture &fireTexture, bool initBrain = true)
        : sprite(shipTexture), fireSprite(fireTexture)
    {
        lasers.resize(laserAngles.size());
        fireSprite.setOrigin({8.f, 2.f});
        fireSprite.setScale({2.0f, 2.0f});
        sf::FloatRect spriteBounds = sprite.getLocalBounds();
        sprite.setOrigin(spriteBounds.getCenter());
        sprite.setScale({2.0f, 2.0f});

        if (initBrain)
        {
            brain = new NeuralNetwork(topology);
        }
    }

    // konstruktor kopiujacy zeby tworzyc nowa rakiete
    Rocket(const Rocket &other) : sprite(other.sprite), fireSprite(other.fireSprite), topology(other.topology)
    {
        lasers = other.lasers;
        velocity = other.velocity;
        fitness = other.fitness;
        dead = other.dead;
        completed = other.completed;
        isThrusting = other.isThrusting;
        visitedCheckpoints = other.visitedCheckpoints;
        timeAlive = other.timeAlive;
        bestDistanceToTarget = other.bestDistanceToTarget;
        stuckCounter = other.stuckCounter;
        lastPosition = other.lastPosition;

        if (other.brain)
        {
            brain = new NeuralNetwork(topology);
            brain->setWeights(other.brain->getWeights());
        }
        else
        {
            brain = nullptr;
        }
    }

    // operator przypisania kopiującego
    Rocket &operator=(const Rocket &other)
    {
        if (this == &other)
            return *this;

        sprite = other.sprite;
        fireSprite = other.fireSprite;
        lasers = other.lasers;
        velocity = other.velocity;
        fitness = other.fitness;
        dead = other.dead;
        completed = other.completed;
        isThrusting = other.isThrusting;
        visitedCheckpoints = other.visitedCheckpoints;
        timeAlive = other.timeAlive;
        bestDistanceToTarget = other.bestDistanceToTarget;
        stuckCounter = other.stuckCounter;
        lastPosition = other.lastPosition;

        delete brain;
        if (other.brain)
        {
            brain = new NeuralNetwork(topology);
            brain->setWeights(other.brain->getWeights());
        }
        else
        {
            brain = nullptr;
        }
        return *this;
    }

    // destruktor
    ~Rocket()
    {
        if (brain)
            delete brain;
    }

    // resetuje rakietę do nowej rundy (pokolenia). Resetuje pozycję, prędkość i flagi życia, ale zachowuje wytrenowany "mózg"
    void reset(sf::Vector2f startPosition, int checkpointsCount)
    {
        dead = false;
        completed = false;
        isThrusting = false;
        sprite.setPosition(startPosition);
        sprite.setRotation(sf::degrees(0.f));
        velocity = {0.f, 0.f};
        fitness = 0.0;
        timeAlive = 0;
        bestDistanceToTarget = 999999.f;
        stuckCounter = 0;
        lastPosition = startPosition;
        visitedCheckpoints.assign(checkpointsCount, false);
    }

    // Odpowiada za podstawową fizykę: dodaje grawitację do prędkości, przesuwa obiekt i wyhamowuje go (tarcie). Zawiera też mechanizm wykrywający "utknięcie" w miejscu (jeśli rakieta się nie rusza, zostaje uśmiercona)
    void updatePhysics()
    {
        if (dead || completed)
            return;

        velocity.y += gravity;
        sprite.move(velocity);
        velocity *= 0.99f;

        // wykrywanie utknięcia
        if (timeAlive % 100 == 0)
        {
            float distMoved = std::sqrt(
                std::pow(sprite.getPosition().x - lastPosition.x, 2) +
                std::pow(sprite.getPosition().y - lastPosition.y, 2));

            if (distMoved < 20.0f)
            {
                stuckCounter++;
                if (stuckCounter >= 3)
                {
                    dead = true;
                }
            }
            else
            {
                stuckCounter = 0;
            }
            lastPosition = sprite.getPosition();
        }
    }

    // główna pętla decyzyjna AI. pobiera dane wejściowe (odczyty laserów, prędkość, kąt do celu), normalizuje je i przepuszcza przez sieć neuronową. wynik sieci decyduje o obrocie i włączeniu silnika.
    void thinkAndMove(const std::vector<sf::CircleShape> &checkpoints, sf::Vector2f finalTarget)
    {
        if (dead || completed)
            return;

        sf::Vector2f currentTarget = finalTarget;
        for (size_t i = 0; i < checkpoints.size(); ++i)
        {
            if (!visitedCheckpoints[i])
            {
                currentTarget = checkpoints[i].getPosition();
                break;
            }
        }

        std::vector<double> inputs;

        // lasery [-1, 1]
        for (const auto &l : lasers)
        {
            double normalized = l.distance / maxLaserDist;
            inputs.push_back(2.0 * normalized - 1.0);
        }

        // prędkość [-1, 1]
        double vx_norm = std::max(-1.0, std::min(1.0, velocity.x / 4.0));
        double vy_norm = std::max(-1.0, std::min(1.0, velocity.y / 4.0));
        inputs.push_back(vx_norm);
        inputs.push_back(vy_norm);

        // Nawigacja
        double dx = currentTarget.x - sprite.getPosition().x;
        double dy = currentTarget.y - sprite.getPosition().y;
        double distToTarget = std::sqrt(dx * dx + dy * dy);

        // kluczowe: śledzimy najlepszy dystans do aktualnego celu
        if (distToTarget < bestDistanceToTarget)
        {
            bestDistanceToTarget = distToTarget;
        }

        // dystans do celu [-1, 1]
        double dist_normalized = std::min(1.0, distToTarget / 1500.0);
        inputs.push_back(2.0 * dist_normalized - 1.0);

        // kąt do celu [-pi, pi]
        double angleToTarget = std::atan2(dy, dx);
        double currentAngle = (sprite.getRotation().asDegrees() - 90.0) * (M_PI_VAL / 180.0);
        double angleDiff = angleToTarget - currentAngle;

        // normalizacja kąta
        while (angleDiff <= -M_PI_VAL)
            angleDiff += 2 * M_PI_VAL;
        while (angleDiff > M_PI_VAL)
            angleDiff -= 2 * M_PI_VAL;


        inputs.push_back(angleDiff / M_PI_VAL);
        inputs.push_back(0.0); // bias

        brain->setCurrentInput(inputs);
        brain->feedForward();
        std::vector<double> outputs = brain->getOutputs();

        bool rotLeft = outputs[0] > 0.0;
        bool rotRight = outputs[1] > 0.0;
        bool thrust = outputs[2] > 0.0;

        // sterowanie
        isThrusting = thrust;
        if (rotLeft)
            sprite.rotate(sf::degrees(-rotationSpeed));
        if (rotRight)
            sprite.rotate(sf::degrees(rotationSpeed));

        // ruch
        if (thrust)
        {
            float angleRad = (sprite.getRotation().asDegrees() - 90.f) * 3.14159f / 180.f;
            velocity.x += std::cos(angleRad) * thrustPower;
            velocity.y += std::sin(angleRad) * thrustPower;

            float offsetDist = 32.0f;
            float fireX = sprite.getPosition().x - std::cos(angleRad) * offsetDist;
            float fireY = sprite.getPosition().y - std::sin(angleRad) * offsetDist;
            fireSprite.setPosition({fireX, fireY});
            fireSprite.setRotation(sprite.getRotation());
        }
    }

    // symuluje działanie czujników odległości. wypuszcza promienie w różnych kierunkach, sprawdza kolizje z przeszkodami i zapisuje odległość do najbliższej ściany.
    void sense(const std::vector<sf::RectangleShape> &obstacles)
    {
        if (dead || completed)
            return;

        sf::Vector2f origin = sprite.getPosition();
        float baseAngle = sprite.getRotation().asDegrees() - 90.f;

        // symulacja czujników odległości
        for (size_t i = 0; i < laserAngles.size(); ++i)
        {
            float rad = (baseAngle + laserAngles[i]) * 3.14159f / 180.f;
            sf::Vector2f rayEnd;
            rayEnd.x = origin.x + std::cos(rad) * maxLaserDist;
            rayEnd.y = origin.y + std::sin(rad) * maxLaserDist;

            float closestDist = maxLaserDist;
            sf::Vector2f closestPoint = rayEnd;
            bool hitSomething = false;

            // sprawdzenie kolizji z przeszkodami
            for (const auto &p : obstacles)
            {
                sf::FloatRect b = p.getGlobalBounds();
                std::vector<std::pair<sf::Vector2f, sf::Vector2f>> walls = {
                    {{b.position.x, b.position.y}, {b.position.x + b.size.x, b.position.y}},
                    {{b.position.x + b.size.x, b.position.y}, {b.position.x + b.size.x, b.position.y + b.size.y}},
                    {{b.position.x + b.size.x, b.position.y + b.size.y}, {b.position.x, b.position.y + b.size.y}},
                    {{b.position.x, b.position.y + b.size.y}, {b.position.x, b.position.y}}};

                sf::Vector2f hitPoint;
                // sprawdzenie kolizji z ścianami
                for (auto &wall : walls)
                {
                    if (getLineIntersection(origin, rayEnd, wall.first, wall.second, hitPoint))
                    {
                        float dist = std::sqrt(std::pow(hitPoint.x - origin.x, 2) + std::pow(hitPoint.y - origin.y, 2));
                        if (dist < closestDist)
                        {
                            closestDist = dist;
                            closestPoint = hitPoint;
                            hitSomething = true;
                        }
                    }
                }
            }
            lasers[i] = {closestPoint, closestDist, hitSomething};
        }
    }

    // sprawdza, czy rakieta przeleciała przez niebieski punkt kontrolny. Jeśli tak, zalicza go i zmusza algorytm do celowania w kolejny punkt
    void checkCheckpoints(const std::vector<sf::CircleShape> &checkpoints)
    {
        if (dead || completed)
            return;

        if (visitedCheckpoints.size() != checkpoints.size())
        {
            visitedCheckpoints.resize(checkpoints.size(), false);
        }

        // sprawdzenie kolizji z punktami kontrolnymi
        sf::FloatRect myBounds = sprite.getGlobalBounds();
        for (size_t i = 0; i < checkpoints.size(); ++i)
        {
            if (visitedCheckpoints[i])
                continue;
            if (myBounds.findIntersection(checkpoints[i].getGlobalBounds()))
            {
                visitedCheckpoints[i] = true;
                // reset dystansu, aby zmusić rakietę do lotu do nowego celu
                bestDistanceToTarget = 999999.f;
            }
        }
    }

    // sprawdza kolizje fizyczne. Jeśli rakieta uderzy w ścianę to ginie. Jeśli dotknie zielonego celu końcowego – wygrywa (oznaczana jako completed)
    void checkCollision(const std::vector<sf::RectangleShape> &obstacles, sf::Vector2f targetPos)
    {
        if (dead || completed)
            return;

        for (const auto &p : obstacles)
        {
            if (sprite.getGlobalBounds().findIntersection(p.getGlobalBounds()))
            {
                dead = true;
            }
        }
        // kolizja z granicami ekranu
        if (sprite.getPosition().x < 0 || sprite.getPosition().x > 1000 ||
            sprite.getPosition().y < 0 || sprite.getPosition().y > 1000)
        {
            dead = true;
        }

        float dist = std::sqrt(std::pow(sprite.getPosition().x - targetPos.x, 2) +
                               std::pow(sprite.getPosition().y - targetPos.y, 2));
        if (dist < 50.0f)
        {
            bool allVisited = true;
            for (auto v : visitedCheckpoints)
                if (!v)
                    allVisited = false;

            if (allVisited)
            {
                completed = true;
                dead = true;
            }
        }
    }

    // Funkcja oceny (Fitness Function). Oblicza wynik rakiety na koniec życia. Punktuje (w kolejności ważności): zdobyte checkpointy, bliskość do aktualnego celu, ukończenie trasy i szybkość przelotu. Decyduje o tym, kto przekaże geny dalej.
    void calcFitness(const std::vector<sf::CircleShape> &checkpoints, sf::Vector2f finalTarget, sf::Vector2f startPos, int maxLifetime)
    {
        fitness = 0.0;

        // 1. Checkpointy (Baza sukcesu) - 10,000 pkt za każdy
        int checkpointCount = 0;
        for (bool visited : visitedCheckpoints)
        {
            if (visited)
                checkpointCount++;
        }
        fitness += checkpointCount * 10000.0;

        // 2. dystans do aktualnego celu (max 2000 pkt)
        // dzięki temu, że max za dystans (2000) < bonus za checkpoint (10000),
        // rakieta, która zdobyła checkpoint zawsze wygrywa z tą, która jest tylko blisko.
        if (bestDistanceToTarget < 99999.f)
        {
            double distReward = std::max(0.0, 2000.0 - (bestDistanceToTarget * 2.0));
            fitness += distReward;
        }

        // 3. bonus za ukończenie i szybkość
        if (completed)
        {
            fitness += 20000.0;
            // im szybciej, tym więcej punktów
            fitness += ((double)maxLifetime / (double)(timeAlive + 1)) * 5000.0;
        }

        // 4. mikro-nagroda za ruch (żeby nie kręciły się w miejscu)
        float totalMovement = std::sqrt(
            std::pow(sprite.getPosition().x - startPos.x, 2) +
            std::pow(sprite.getPosition().y - startPos.y, 2));
        fitness += totalMovement * 0.1;
    }

    // rysuje rakietę na ekranie. Jeśli silnik jest włączony, dorysowuje ogień. Najlepsza rakieta w populacji jest wyróżniona kolorem zielonym
    void draw(sf::RenderWindow &win, bool best)
    {
        if (dead && !best)
            return;
        if (isThrusting && !dead)
            win.draw(fireSprite);
        if (best)
            sprite.setColor(sf::Color::Green);
        else
            sprite.setColor(sf::Color(255, 255, 255, 100));
        win.draw(sprite);
    }
};

#endif // ROCKET_H
