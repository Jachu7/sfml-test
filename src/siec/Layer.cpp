#include "Layer.h"

// nowy obiekt Matrix (1 wiersz, n kolumn) i przepisuje do niego surowe wartości wszystkich neuronów w tej warstwie
Matrix *Layer::matrixifyVals()
{
    Matrix *m = new Matrix(1, this->neurons.size(), false);
    for (int i = 0; i < this->neurons.size(); i++)
    {
        m->setValue(
            0,
            i,
            this->neurons.at(i)->getValue());
    }
    return m;
}

// nowy obiekt Matrix i przepisuje do niego aktywowane wartości neuronów (po funkcji Softsign). Używane jako wejście do obliczeń dla kolejnej warstwy.
Matrix *Layer::matrixifyActivatedVals()
{
    Matrix *m = new Matrix(1, this->neurons.size(), false);
    for (int i = 0; i < this->neurons.size(); i++)
    {
        m->setValue(
            0,
            i,
            this->neurons.at(i)->getActiveValue());
    }
    return m;
}

// Konstruktor warstwy. Tworzy zadaną liczbę obiektów Neuron i przechowuje je w wektorze
Layer::Layer(int size)
{
    this->size = size;
    for (int i = 0; i < size; i++)
    {
        Neuron *n = new Neuron(0.00);
        this->neurons.push_back(n);
    }
}

// Ustawia wartość konkretnego neuronu (o indeksie i) w danej warstwie
void Layer::setValue(int i, double v)
{
    this->neurons.at(i)->setValue(v);
}
