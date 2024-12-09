#ifndef TIMEMANAGER_HPP
#define TIMEMANAGER_HPP

#include <string>
#include <sstream>
#include <iomanip>

class TimeManager {
private:
    float elapsedTime = 0.0f;         // Elapsed time in seconds
    int currentDay = 1;               // Current simulation day
    int societyIteration = 0;         // Society generation or iteration
    static constexpr float SECONDS_IN_A_DAY = 86400.0f;
    static constexpr float TIME_SCALE = 480.0f; // 1 day = 3 minutes

public:
    TimeManager() = default;

    void update(float deltaTime) {
        elapsedTime += deltaTime * TIME_SCALE;
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
            int minutes = static_cast<int>((elapsedTime / 60.0f)) % 60;
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
        societyIteration = 0;
    }
};

#endif
