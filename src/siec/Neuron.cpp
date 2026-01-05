#include "Neuron.h"
#include <cmath>

// Ustawia surową wartość neuronu i natychmiast wywołuje funkcję activate() aby przeliczyć wartość aktywowaną
void Neuron::setValue(double value)
{
    this->value = value;
    activate();
}

// konstruktor
Neuron::Neuron(double value)
{
    this->value = value;
    activate();
}
// funkcja softsign do sprowadzania wartosci do zakresu (-1,1)
void Neuron::activate()
{
    this->activatedValue = this->value / (1 + abs(this->value));
}
