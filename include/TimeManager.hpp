#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <string>

class TimeManager {
private:
    float elapsedTime;  // Elapsed time in seconds
    int currentDay;     // Current day in the simulation
    int societyIteration; // Iteration or "generation" of the society
    static constexpr float SECONDS_IN_A_DAY = 86400.0f; // 24 hours in seconds
    static constexpr float TIME_SCALE = 480.0f;         // Scaled to 3 minutes (180 seconds) = 1 day

public:
    TimeManager();  // Constructor to initialize values

    // Update elapsed time
    void update(float deltaTime);

    // Getters
    int getCurrentDay() const;
    int getSocietyIteration() const;
    std::string getFormattedTime() const;
    float getElapsedTime() const;

    // Increment society iteration manually
    void incrementSocietyIteration();

    // Reset (optional, for restarting simulation)
    void reset();
};

#endif
