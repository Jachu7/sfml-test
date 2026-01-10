#ifndef GENETIC_ALGORITHM_H
#define GENETIC_ALGORITHM_H

#include <vector>
#include <algorithm>
#include <cstdlib>

#include "Rocket.h"
#include "Utils.h"

// Konfiguracja
const int POPULATION_SIZE = 100;      // ilość rakiet
const int MUTATION_RATE = 5;          // 5%
const double MUTATION_STRENGTH = 0.1; // 0-1
const int LIFETIME = 2000;           // Czas trwania rundy

// Ewolucja -
// Implementuje Algorytm Genetyczny.
// Sortuje rakiety od najlepszej do najgorszej.
// Przepisuje najlepsze jednostki bez zmian do nowej populacji (Elityzm).
// Tworzy resztę nowej populacji poprzez mieszanie wag dwóch rodziców (Crossover) i losowe zmiany wag (Mutacja).
inline std::vector<Rocket> evolve(std::vector<Rocket> &oldPop, sf::Texture &t, sf::Texture &ft, sf::Vector2f startPos, int cpCount)
{
    // Inicjalizacja nowej populacji
    std::vector<Rocket> newPop;
    newPop.reserve(POPULATION_SIZE);

    // Sortowanie populacji
    std::sort(oldPop.begin(), oldPop.end(), [](const Rocket &a, const Rocket &b)
              { return a.fitness > b.fitness; });

    // 1. ELITYZM - Zwiększony do 8, aby zachować stabilność
    int eliteCount = 8;
    for (int i = 0; i < std::min(eliteCount, (int)oldPop.size()); ++i)
    {
        // Klonowanie najlepszych rakiet
        Rocket elita(t, ft, true);
        elita.brain->setWeights(oldPop[i].brain->getWeights());
        elita.reset(startPos, cpCount);
        newPop.push_back(elita);
    }

    // 2. RESZTA POPULACJI (Crossover + Mutacja)
    while (newPop.size() < POPULATION_SIZE)
    {
        // Selekcja Turniejowa
        int best1 = rand() % oldPop.size();
        for (int i = 0; i < 5; ++i)
        { // Turniej 5 osobników
            int cand = rand() % oldPop.size();
            if (oldPop[cand].fitness > oldPop[best1].fitness)
                best1 = cand;
        }
        int best2 = rand() % oldPop.size();
        for (int i = 0; i < 5; ++i)
        {
            int cand = rand() % oldPop.size();
            if (oldPop[cand].fitness > oldPop[best2].fitness)
                best2 = cand;
        }

        // Tworzenie nowej rakiety poprzez mieszanie wag dwóch rodziców
        Rocket child(t, ft, true);
        std::vector<double> genes1 = oldPop[best1].brain->getWeights();
        std::vector<double> genes2 = oldPop[best2].brain->getWeights();
        std::vector<double> childGenes;

        // Mieszanie wag rodziców i mutacja
        for (size_t i = 0; i < genes1.size(); ++i)
        {
            // Losowy wybór rodzica
            double gene = (rand() % 2 == 0) ? genes1[i] : genes2[i];
            // Mutacja
            if ((rand() % 100) < MUTATION_RATE)
            {
                gene += randomRange(-MUTATION_STRENGTH, MUTATION_STRENGTH);
                gene = std::max(-1.0, std::min(1.0, gene));
            }
            childGenes.push_back(gene);
        }

        // Ustawienie wag nowej rakiety
        child.brain->setWeights(childGenes);
        child.reset(startPos, cpCount);
        newPop.push_back(child);
    }

    return newPop;
}

#endif // GENETIC_ALGORITHM_H
