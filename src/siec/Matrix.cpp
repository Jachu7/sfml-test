#include "Matrix.h"

#include <random>

// Generuje losową liczbę zmiennoprzecinkową z zakresu od -1 do 1
double Matrix::generateRandomNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    return dis(gen);
}

// Konstruktor macierzy. Alokuje pamięć dla wektora dwuwymiarowego o zadanych wymiarach
Matrix::Matrix(int numRows, int numCols, bool isRandom)
{
    this->numRows = numRows;
    this->numCols = numCols;

    for (int i = 0; i < numRows; i++)
    {
        std::vector<double> colValues;

        for (int j = 0; j < numCols; j++)
        {
            double r = 0.00;
            if (isRandom)
            {
                r = this->generateRandomNumber();
            }
            colValues.push_back(r);
        }
        this->values.push_back(colValues);
    }
}