#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <string>
#include <sstream>
#include <iomanip>

class TimeManager {
private:
    float elapsedTime = 0.0f;
    int currentDay = 1;
    int societyIteration = 0;
    
    static constexpr float SECONDS_IN_A_DAY = 86400.0f;
    static constexpr float TIME_SCALE = 480.0f;

public:
    TimeManager() = default;

    // Accept simulation speed multiplier
    void update(float deltaTime, float simulationSpeed = 1.0f) {
        // Apply both time scale and simulation speed
        elapsedTime += deltaTime * TIME_SCALE * simulationSpeed;

        if (elapsedTime >= SECONDS_IN_A_DAY) {
            elapsedTime -= SECONDS_IN_A_DAY;
            currentDay++;
        }
    }

    int getCurrentDay() const { return currentDay; }
    int getSocietyIteration() const { return societyIteration; }
    float getElapsedTime() const { return elapsedTime; }
    void incrementSocietyIteration() { societyIteration++; }

    mutable std::string cachedFormattedTime;
    mutable float lastCachedElapsedTime = -1.0f;

    std::string getFormattedTime() const {
        if (elapsedTime != lastCachedElapsedTime) {
            int hours = static_cast<int>((elapsedTime / 3600.0f)) % 24;
            int minutes = (static_cast<int>(elapsedTime) % 3600) / 60;

            std::ostringstream oss;
            oss << std::setw(2) << std::setfill('0') << hours << ":"
                << std::setw(2) << std::setfill('0') << minutes;

            cachedFormattedTime = oss.str();
            lastCachedElapsedTime = elapsedTime;
        }

        return cachedFormattedTime;
    }

    void reset() {
        elapsedTime = 0.0f;
        currentDay = 1;
    }
};

#endif