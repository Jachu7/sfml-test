#ifndef CMAKESFMLPROJECT_NEURAL_NETWORK_H
#define CMAKESFMLPROJECT_NEURAL_NETWORK_H

#include <iostream>
#include <vector>
#include "Matrix.h"
#include "Layer.h"
#include "SFML/Window/Keyboard.hpp"

class NeuralNetwork {
public:
    NeuralNetwork(std::vector<int> topology);
    void setCurrentInput(std::vector<double> input);
    void printToConsole();
    void feedForward();

    Matrix *getNeuronMatrix(int index){return this->layers.at(index)->matrixifyVals();};
    Matrix *getActivatedNeuronMatrix(int index){return this->layers.at(index)->matrixifyActivatedVals();};
    Matrix *getDerivedNeuronMatrix(int index){return this->layers.at(index)->matrixifyDerivedVals();};
    Matrix *getWeightMatrix(int index){return this->weightMatrices.at(index);};

    void setNeuronValue(int indexLayer, int indexNeuron, double val){this->layers.at(indexLayer)->setValue(indexNeuron, val);};

private:
    int topologySize;
    std::vector<int> topology;
    std::vector<Layer *> layers;
    std::vector<Matrix *> weightMatrices;
    std::vector<double> input;
};


#endif //CMAKESFMLPROJECT_NEURAL_NETWORK_H