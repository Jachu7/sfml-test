#include "NeuralNetwork.h"
#include "utils/MultiplyMatrix.h"

NeuralNetwork::~NeuralNetwork()
{
    for (auto l : layers)
        delete l;
    for (auto m : weightMatrices)
        delete m;
    // gradientMatrices w tym kodzie nie są inicjalizowane w konstruktorze,
    // ale jeśli by były, też trzeba je usunąć.
}

void NeuralNetwork::setErrors()
{
    if (this->target.size() == 0)
    {
        std::cerr << "No target for this neural network" << std::endl;
        assert(false);
    }

    if (this->target.size() != this->layers.at(this->layers.size() - 1)->getNeurons().size())
    {
        std::cerr << "Target size is not the same as output layer size: " << this->layers.at(this->layers.size() - 1)->getNeurons().size() << std::endl;
        assert(false);
    }

    this->error = 0.00;
    int outputLayerIndex = this->layers.size() - 1;
    std::vector<Neuron *> outputNeurons = this->layers.at(outputLayerIndex)->getNeurons();
    errors.resize(target.size()); // Ensure errors vector has correct size
    for (int i = 0; i < target.size(); i++)
    {
        double tempErr = (outputNeurons.at(i)->getActiveValue() - target.at(i));
        errors.at(i) = tempErr;
        this->error += tempErr;
    }

    historicalErrors.push_back(this->error);
}

void NeuralNetwork::feedForward()
{
    for (int i = 0; i < (this->layers.size() - 1); i++)
    {
        Matrix *a = this->getNeuronMatrix(i);

        if (i != 0)
        {
            // Musimy zwolnić pamięć po poprzednim matrixifyVals, jeśli matrixify tworzy nowy obiekt
            // W Twojej implementacji matrixify tworzy 'new Matrix', więc tutaj powstawały wycieki pamięci.
            // Dla uproszczenia w GA zostawiamy jak jest, ale warto pamiętać o delete.
            Matrix *temp = a;
            a = this->getActivatedNeuronMatrix(i);
            delete temp;
        }

        Matrix *b = this->getWeightMatrix(i);
        Matrix *c = (new utils::MultiplyMatrix(a, b))->execute();

        // Sprzątanie po macierzy a (jeśli była alokowana dynamicznie w pętli)
        delete a;

        for (int c_index = 0; c_index < c->getNumCols(); c_index++)
        {
            this->setNeuronValue(i + 1, c_index, c->getValue(0, c_index));
        }
        delete c; // Sprzątamy wynik mnożenia
    }
}

void NeuralNetwork::printToConsole()
{
    for (int i = 0; i < this->layers.size(); i++)
    {
        std::cout << "Layer " << i << ":\n";
        Matrix *m = (i == 0) ? this->layers.at(i)->matrixifyVals() : this->layers.at(i)->matrixifyActivatedVals();
        m->printToConsole();
        delete m; // Sprzątanie

        std::cout << "===========" << std::endl;
        if (i < this->layers.size() - 1)
        {
            std::cout << "Weight Matrix: " << i << std::endl;
            this->getWeightMatrix(i)->printToConsole();
        }
        std::cout << "===========" << std::endl;
    }
}

void NeuralNetwork::setCurrentInput(std::vector<double> input)
{
    this->input = input;
    for (int i = 0; i < input.size(); i++)
    {
        this->layers.at(0)->setValue(i, input.at(i));
    }
}

NeuralNetwork::NeuralNetwork(std::vector<int> topology)
{
    this->topology = topology;
    this->topologySize = topology.size();
    for (int i = 0; i < topology.size(); i++)
    {
        Layer *l = new Layer(topology.at(i));
        this->layers.push_back(l);
    }
    for (int i = 0; i < (topology.size() - 1); i++)
    {
        Matrix *m = new Matrix(topology.at(i), topology.at(i + 1), true);
        this->weightMatrices.push_back(m);
    }
}

// --- IMPLEMENTACJA GA ---

std::vector<double> NeuralNetwork::getWeights() const
{
    std::vector<double> weights;
    for (Matrix *m : weightMatrices)
    {
        for (int r = 0; r < m->getNumRows(); r++)
        {
            for (int c = 0; c < m->getNumCols(); c++)
            {
                weights.push_back(m->getValue(r, c));
            }
        }
    }
    return weights;
}

void NeuralNetwork::setWeights(const std::vector<double> &weights)
{
    int k = 0;
    for (Matrix *m : weightMatrices)
    {
        for (int r = 0; r < m->getNumRows(); r++)
        {
            for (int c = 0; c < m->getNumCols(); c++)
            {
                if (k < weights.size())
                {
                    m->setValue(r, c, weights[k]);
                    k++;
                }
            }
        }
    }
}

std::vector<double> NeuralNetwork::getOutputs()
{
    std::vector<double> ret;
    Layer *outputLayer = layers.at(layers.size() - 1);
    for (auto neuron : outputLayer->getNeurons())
    {
        ret.push_back(neuron->getActiveValue());
    }
    return ret;
}