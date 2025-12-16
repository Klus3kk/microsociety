#include "SimulationConfig.hpp"

SimulationConfig& getSimulationConfig() {
    static SimulationConfig config;
    return config;
}

