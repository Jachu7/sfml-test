# 🚀 Neural Network Rockets

<div align="center">

**An evolutionary simulation where autonomous agents learn to navigate through obstacles using neural networks and genetic algorithms**

[About](#-about-the-project) • [Features](#%EF%B8%8F-tech-stack--features) • [How to Build](#-how-to-build-and-run) • [How it Works](#-how-it-works) • [Acknowledgements](#-inspiration--acknowledgements)

---

</div>

## 🎮 Demo
<p align="center">
  <img src="images/image.png" alt="Neural Network Rockets Demo" width="700"/>
</p>
<p align="center">
  <em>Rockets learning to navigate through obstacles using evolved neural networks</em>
</p>


## 📖 About the Project

Neural Network Rockets is a simulation project where autonomous agents (rockets) learn to navigate through an obstacle course to reach a target. The project implements a **Neural Network library from scratch** (without using external AI frameworks like TensorFlow or PyTorch) and utilizes a **Genetic Algorithm** to evolve the population over generations.

The rockets use raycasting sensors (lasers) to "see" their surroundings and make decisions based on their neural network's output.

---

## 🎓 Academic Context

This project was created as a final assignment for the **Programming 1 course.**

-   **University:** Silesian University of Technology
-   **Faculty:** Faculty of Applied Mathematics
-   **Major:** Computer Science (1st Year)

---

## 🛠️ Tech Stack & Features

### Technologies

-   **Language:** C++
-   **Graphics:** SFML (Simple and Fast Multimedia Library)
-   **Build System:** CMake

### Key Concepts

-   **Neural Network:** Fully connected Multi-Layer Perceptron (MLP) built from scratch using matrix operations
-   **Genetic Algorithm:** Implements Selection, Crossover, Mutation, and Elitism
-   **Raycasting:** Custom collision detection sensors for the agents

---

## 💡 Inspiration & Acknowledgements

The structure of the project and the implementation of the core Neural Network were inspired by the **"Live Coding 2: C++ Neural Network"** series by the YouTube channel **devlogs**.

This tutorial series was instrumental in understanding how to structure the matrix math and the architecture required to build a neural network from scratch in C++.

**Channel:** [devlogs](https://www.youtube.com/@devlogs1785)

---

## 🚀 How to Build and Run

### Prerequisites

-   C++ Compiler (supporting C++17)
-   CMake (Version 3.10 or higher)
-   SFML Library (Must be installed on your system)

### 🐧 Linux (Terminal)

1. **Clone the repository:**

    ```bash
    git clone https://github.com/Jachu7/Neural-Network-Rockets.git
    cd Neural-Network-Rockets
    ```

2. **Create a build directory:**

    ```bash
    mkdir build
    cd build
    ```

3. **Generate Makefiles using CMake:**

    ```bash
    cmake ..
    ```

4. **Compile the project:**

    ```bash
    make
    ```

5. **Run the application:**
    ```bash
    cd bin
    ./main
    ```

> **Note:** If the bin folder is not created, try running `./NeuralNetworkRockets` directly in the build folder, depending on your CMake configuration.

### 🪟 Windows

The easiest way to build and run this project on Windows is using **CLion** (JetBrains) or **Visual Studio**, as they have built-in CMake support.

#### Using CLion:

1. Open CLion
2. Select "Open" or "Open from VCS" and select the project folder
3. CLion will automatically detect the `CMakeLists.txt` file and load the project structure
4. Wait for the indexing and CMake configuration to finish (you should see "CMake: Finish" in the bottom console)
5. Click the green **Run** (Play) button in the top right corner

> **Note:** Ensure that SFML is properly linked in your environment or installed via a package manager like vcpkg.

---

## 📂 Project Structure

```
src/
├── main.cpp              # Main simulation loop, SFML rendering
├── Rocket.h              # Rocket agent: physics, sensors, fitness
├── GeneticAlgorithm.h    # Evolution: selection, crossover, mutation
├── LaserReading.h        # Laser sensor data structure
├── Utils.h               # Math utilities (line intersection, random)
├── assets/
│   └── Roboto_Condensed-Medium.ttf   # UI font
├── img/
│   ├── rakieta.png       # Rocket sprite
│   └── ogien.png         # Thrust flame sprite
└── siec/                 # Neural Network Library
    ├── NeuralNetwork.h/cpp   # Network topology & feedforward
    ├── Layer.h/cpp           # Layer of neurons
    ├── Neuron.h/cpp          # Single neuron with activation
    ├── Matrix.h/cpp          # Matrix operations for weights
    └── utils/
        └── MultiplyMatrix.h/cpp  # Matrix multiplication utility
```

### Core Components

-   **main.cpp:** Main simulation loop and SFML rendering
-   **Rocket.h:** Agent structure with physics, raycasting sensors, neural network brain, and fitness calculation
-   **GeneticAlgorithm.h:** Implements evolution with elitism, tournament selection, crossover, and mutation
-   **LaserReading.h:** Data structure for laser sensor readings
-   **Utils.h:** Helper functions for line intersection detection and random number generation

### Neural Network Library (`/siec` folder)

-   **NeuralNetwork:** Manages the topology of the network. Handles feedForward (passing data from input to output) and manages layers
-   **Layer:** Represents a layer of neurons
-   **Neuron:** A single unit that holds a value and an activation function (Softsign)
-   **Matrix:** A custom math class to handle weights and matrix multiplication
-   **MultiplyMatrix:** Utility class for matrix operations

---

## 🧠 How it Works

### 1. **Initialization**

100 rockets are spawned with random neural weights.

### 2. **Simulation**

-   Inputs (distances, velocity, angle to target) are fed into the Neural Network
-   The Network outputs control signals (Thrust, Rotate Left, Rotate Right)

### 3. **Evaluation**

-   If a rocket hits a wall, it dies
-   If a rocket reaches the target, it succeeds
-   Fitness is calculated based on: Checkpoints reached, distance to target, and completion time

### 4. **Evolution**

-   The best rockets are kept (Elitism)
-   New rockets are created by mixing the "brains" (weights) of the best performers
-   Small random changes are applied (Mutation) to discover new strategies

### 5. **Repeat**

The cycle continues, and rockets get smarter every generation.

---

## 📊 Training Performance

-   **Training Duration:** On average, the population learns to navigate the course effectively within **15 to 50 generations**
-   **Success Rate:** A success rate of 30-60 rockets out of 100 reaching the target is perfectly acceptable during training. This level of performance demonstrates that the neural networks have learned viable strategies, and in real-world applications, such success rates would be sufficient for practical deployment

---

## ⭐ Final Notes

This project demonstrates how:

-   Neural networks can be implemented from scratch
-   Evolutionary algorithms can replace traditional training methods
-   Complex behavior can emerge from simple rules

**If you find this project interesting, feel free to ⭐ the repository!**

---

<div align="center">

**Made with ❤️ as a Programming 1 final project**

</div>
