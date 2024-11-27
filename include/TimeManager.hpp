#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <string>

class TimeManager {
private:
    float elapsedTime;  // Elapsed time in seconds
    int currentDay;     // Current day in the simulation
    int societyIteration; // Iteration or "generation" of the society

public:
    TimeManager();  // Constructor to initialize values

    // Update elapsed time
    void update(float deltaTime);

    // Getters
    int getCurrentDay() const;
    int getSocietyIteration() const;
    std::string getFormattedTime() const;

    // Reset (optional, for restarting simulation)
    void reset();
};

#endif
