#include "Matrix.h"

#include <random>
#include <iostream> // Dodane dla cout

Matrix *Matrix::transpose()
{
    Matrix *m = new Matrix(this->numCols, this->numRows, false);
    for (int i = 0; i < this->numRows; i++)
    {
        for (int j = 0; j < this->numCols; j++)
        {
            m->setValue(j, i, this->getValue(i, j));
        }
    }
    return m;
}

double Matrix::generateRandomNumber()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    // ZMIANA: Wagi muszą być ujemne i dodatnie (-1 do 1)
    // Wcześniej było (0, 1), co uniemożliwiało hamowanie sygnałów!
    std::uniform_real_distribution<> dis(-1.0, 1.0);

    return dis(gen);
}

void Matrix::printToConsole()
{
    for (int i = 0; i < numRows; i++)
    {
        for (int j = 0; j < numCols; j++)
        {
            std::cout << this->values.at(i).at(j) << "\t\t";
        }
        std::cout << std::endl;
    }
}

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