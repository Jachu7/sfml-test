#ifndef CMAKESFMLPROJECT_NEURON_H
#define CMAKESFMLPROJECT_NEURON_H

#include <iostream>
#include <math.h>

class Neuron
{
public:
    Neuron(double value);
    void setValue(double value);
    void activate();

    // getters
    double getValue()
    {
        return this->value;
    }
    double getActiveValue()
    {
        return this->activatedValue;
    }

private:
    double value;
    double activatedValue;
};

#endif // CMAKESFMLPROJECT_NEURON_H