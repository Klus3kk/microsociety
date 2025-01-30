#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <string>
#include <sstream>
#include <iomanip>

// Manages in-game time, tracking elapsed time, days, and simulation iterations
class TimeManager {
private:
    float elapsedTime = 0.0f;         // Tracks the total elapsed time in seconds
    int currentDay = 1;               // Current simulation day (starts from 1)
    int societyIteration = 0;         // Keeps track of the number of iterations (e.g., cycles in the simulation)

    static constexpr float SECONDS_IN_A_DAY = 86400.0f;  // Number of seconds in a full day
    static constexpr float TIME_SCALE = 480.0f;          // Speed multiplier (1 day = 3 minutes real-time)

public:
    TimeManager() = default; // Default constructor

    // Updates the elapsed time and increments the day when needed
    void update(float deltaTime) {
        elapsedTime += deltaTime * TIME_SCALE; // Scale time based on simulation speed

        if (elapsedTime >= SECONDS_IN_A_DAY) { // Check if a full day has passed
            elapsedTime -= SECONDS_IN_A_DAY;   // Reset elapsed time for the next day
            currentDay++;                      // Move to the next day
        }
    }

    // Returns the current day in the simulation
    int getCurrentDay() const { return currentDay; }

    // Returns the current society iteration
    int getSocietyIteration() const { return societyIteration; }

    // Returns the elapsed time in the current day
    float getElapsedTime() const { return elapsedTime; }

    // Increments the society iteration counter (useful for tracking generations)
    void incrementSocietyIteration() { societyIteration++; }

    // Cached formatted time for optimization
    mutable std::string cachedFormattedTime;
    mutable float lastCachedElapsedTime = -1.0f;

    // Formats and returns the current time of day (HH:MM)
    std::string getFormattedTime() const {
        // Only recompute if the elapsed time has changed since the last call
        if (elapsedTime != lastCachedElapsedTime) {
            int hours = static_cast<int>((elapsedTime / 3600.0f)) % 24; // Convert elapsed seconds to hours (mod 24)
            int minutes = static_cast<int>((elapsedTime / 60.0f)) % 60; // Convert elapsed seconds to minutes (mod 60)

            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << hours << ":"  // Format hours (00-23)
                << std::setw(2) << std::setfill('0') << minutes;       // Format minutes (00-59)

            cachedFormattedTime = oss.str(); // Store the formatted time
            lastCachedElapsedTime = elapsedTime; // Cache the last checked time
        }

        return cachedFormattedTime; // Return the formatted time as a string
    }

    // Resets the time manager to its initial state
    void reset() {
        elapsedTime = 0.0f;
        currentDay = 1;
    }
};

#endif 
