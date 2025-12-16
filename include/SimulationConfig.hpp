#ifndef SIMULATION_CONFIG_HPP
#define SIMULATION_CONFIG_HPP

// SimulationConfig holds tunable behaviour parameters for the simulation.
// These values will eventually be loaded from external config (JSON/YAML).
struct SimulationConfig {
    // Energy / health dynamics
    float energyRegenRate      = 1.0f;  // Energy regeneration per time unit
    float healthDecayRate      = 0.5f;  // Health decay per time unit when conditions are bad

    // Consumption effects
    float foodEnergyBoost      = 20.0f; // Energy boost from consuming food
    float waterEnergyBoost     = 15.0f; // Energy boost from consuming water

    // Action economy
    float actionEnergyCost     = 10.0f; // Baseline energy cost per heavy action
    float restEnergyRecovery   = 25.0f; // Energy recovered when resting
    float minEnergyToAct       = 10.0f; // Minimum energy required to perform heavy actions
};

// Accessor for global simulation config
SimulationConfig& getSimulationConfig();

#endif

