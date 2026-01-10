#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <optional>
#include <limits>
#include <iostream>
#include <random>
#include <algorithm>
#include <numeric>

#include "siec/Neuron.h"
#include "siec/Matrix.h"
#include "siec/NeuralNetwork.h"
#include "siec/utils/MultiplyMatrix.h"

#include "LaserReading.h"
#include "Utils.h"
#include "Rocket.h"
#include "GeneticAlgorithm.h"

int main()
{
    // Inicjalizacja okna oraz tekstu
    auto window = sf::RenderWindow(sf::VideoMode({1000u, 1000u}), "Symulacja algorytmu genetycznego - Neural Network Rockets C++");
    window.setFramerateLimit(200);
    sf::Font font;
    if (!font.openFromFile("../../src/assets/Roboto_Condensed-Medium.ttf"))
    {
    }

    sf::Text textGen(font);
    textGen.setCharacterSize(24);
    textGen.setFillColor(sf::Color::Black);
    textGen.setPosition({10.f, 10.f});

    std::vector<sf::RectangleShape> przeszkody;
    auto dodajPrzeszkode = [&](sf::Vector2f size, sf::Vector2f pos)
    {
        sf::RectangleShape p(size);
        p.setFillColor(sf::Color::Black);
        p.setPosition(pos);
        przeszkody.push_back(p);
    };

    // Dodawanie przeszkód i krawędzi
    dodajPrzeszkode({600.f, 20.f}, {400.f, 450.f});
    dodajPrzeszkode({600.f, 20.f}, {0.f, 220.f});
    dodajPrzeszkode({600.f, 20.f}, {0.f, 720.f});
    dodajPrzeszkode({1000.f, 10.f}, {0.f, 0.f});
    dodajPrzeszkode({1000.f, 10.f}, {0.f, 990.f});
    dodajPrzeszkode({10.f, 1000.f}, {0.f, 0.f});
    dodajPrzeszkode({10.f, 1000.f}, {990.f, 0.f});

    std::vector<sf::CircleShape> checkpoints;
    auto dodajCheckpoint = [&](sf::Vector2f pos)
    {
        sf::CircleShape ch({70.f});
        ch.setFillColor(sf::Color(100, 150, 200, 150));
        ch.setPosition(pos);
        checkpoints.push_back(ch);
    };

    // Dodawanie checkpointów   
    dodajCheckpoint({700.f, 620.f});
    dodajCheckpoint({100.f, 430.f});
    dodajCheckpoint({700.f, 130.f});

    // Finalny checkpoint
    sf::CircleShape cel({50.f});
    cel.setFillColor(sf::Color::Green);
    cel.setOrigin({50.f, 50.f});
    cel.setPosition({100.f, 100.f});

    // Wczytanie tekstur
    sf::Texture texture;
    if (!texture.loadFromFile("../../src/img/rakieta.png"))
        return -1;
    sf::Texture fireTexture;
    if (!fireTexture.loadFromFile("../../src/img/ogien.png"))
        return -1;

    sf::Vector2f startPos = {100.f, 900.f};

    // Inicjalizacja populacji
    std::vector<Rocket> population;
    for (int i = 0; i < POPULATION_SIZE; ++i)
    {
        Rocket r(texture, fireTexture, true);
        r.reset(startPos, checkpoints.size());
        population.push_back(r);
    }

    // Inicjalizacja zmiennych
    int generation = 1;
    int timer = 0;
    bool showLasers = false;  // Toggle widoku laserów klawiszem L

    // Główna pętla
    while (window.isOpen())
    {
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
                window.close();
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                if (keyPressed->code == sf::Keyboard::Key::L)
                    showLasers = !showLasers;
            }
        }

        // Sprawdzenie czy wszystkie rakiety są martwe
        bool allDead = true;
        for (auto &rocket : population)
        {
            // Jeśli rakieta jest żywa lub ukończona => aktualizacja
            if (!rocket.dead && !rocket.completed)
            {
                rocket.timeAlive++;
                rocket.sense(przeszkody);
                rocket.checkCheckpoints(checkpoints);
                rocket.thinkAndMove(checkpoints, cel.getPosition());
                rocket.updatePhysics();
                rocket.checkCollision(przeszkody, cel.getPosition());
                allDead = false;
            }
        }

        timer++;

        // Jeśli wszystkie rakiety są martwe lub osiagnieto limit życia
        if (allDead || timer > LIFETIME)
        {
            double maxFit = -std::numeric_limits<double>::infinity();
            double sumFit = 0.0;
            int completedCount = 0;
            int maxCheckpoints = 0;

            // Obliczenie fitnessu dla każdej rakiety
            for (auto &r : population)
            {
                r.calcFitness(checkpoints, cel.getPosition(), startPos, LIFETIME);
                if (r.fitness > maxFit)
                    maxFit = r.fitness;
                sumFit += r.fitness;
                if (r.completed)
                    completedCount++;

                // Obliczenie ilości odwiedzonych checkpointów
                int cpCount = 0;
                for (bool v : r.visitedCheckpoints)
                    if (v)
                        cpCount++;
                maxCheckpoints = std::max(maxCheckpoints, cpCount);
            }

            // Wypisanie statystyk
            std::cout << "=== GEN " << generation << " ===" << std::endl;
            std::cout << "  Max Fitness: " << (long)maxFit << std::endl;
            std::cout << "  Ukończone: " << completedCount << "/" << POPULATION_SIZE << std::endl;
            std::cout << "  Max CP: " << maxCheckpoints << "/3" << std::endl;

            // Wyznaczenie nowej populacji
            population = evolve(population, texture, fireTexture, startPos, checkpoints.size());
            generation++;
            timer = 0;
        }

        // Rysowanie
        window.clear(sf::Color::White);
        for (const auto &cp : checkpoints)
            window.draw(cp);
        window.draw(cel);
        for (const auto &p : przeszkody)
            window.draw(p);

        // Znajdź najlepszą do rysowania (żywą lub martwą z najlepszym wynikiem)
        int bestIdx = 0;
        double bestScore = -1.0;

        // Prostym sposobem wizualizacji jest pokazanie tej, która ma najwięcej CP
        // i jest najbliżej celu (liczone na bieżąco dla wizualizacji)
        for (size_t i = 0; i < population.size(); ++i)
        {
            // Jeśli rakieta jest martwa, przechodzimy do następnej
            if (population[i].dead)
                continue;

            int cp = 0;
            // Obliczenie ilości odwiedzonych checkpointów
            for (auto v : population[i].visitedCheckpoints)
                if (v)
                    cp++;
            // Obliczenie wyniku
            double score = cp * 10000.0 - population[i].bestDistanceToTarget;
            if (score > bestScore)
            {
                bestScore = score;
                bestIdx = i;
            }
        }

        // Rysowanie każdej rakiety
        for (size_t i = 0; i < population.size(); ++i)
        {
            population[i].draw(window, (i == bestIdx));
        }

        // Rysowanie laserów dla każdej żywej rakiety
        if (showLasers)
        {
            for (const auto& rocket : population)
            {
                if (rocket.dead)
                    continue;
                for (const auto& laser : rocket.lasers)
                {
                    sf::Color laserColor = laser.hit ? sf::Color::Red : sf::Color(150, 150, 150, 100);  // Czerwony gdy trafiony, szary gdy nie
                    sf::Vertex line[] =
                    {
                        sf::Vertex{rocket.sprite.getPosition(), laserColor},
                        sf::Vertex{laser.endPoint, laserColor}
                    };
                    window.draw(line, 2, sf::PrimitiveType::Lines);
                }
            }
        }

        // Rysowanie statystyk
        textGen.setString("Gen: " + std::to_string(generation) + " | Step: " + std::to_string(timer) + " | [L] toggle raycasts");
        window.draw(textGen);
        window.display();
    }
    return 0;
}