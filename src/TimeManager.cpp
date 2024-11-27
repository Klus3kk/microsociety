#include "TimeManager.hpp"
#include <iomanip>
#include <sstream>

TimeManager::TimeManager() 
    : elapsedTime(0.0f), currentDay(1), societyIteration(1) {}

void TimeManager::update(float deltaTime) {
    elapsedTime += deltaTime * TIME_SCALE;  // Accelerate time progression

    // Increment day after scaled "day" duration
    if (elapsedTime >= SECONDS_IN_A_DAY) {
        elapsedTime -= SECONDS_IN_A_DAY;  // Reset elapsed time for the next day
        currentDay++;
        societyIteration++;
    }
}

int TimeManager::getCurrentDay() const {
    return currentDay;
}

int TimeManager::getSocietyIteration() const {
    return societyIteration;
}

std::string TimeManager::getFormattedTime() const {
    // Calculate scaled time components
    int hours = static_cast<int>(elapsedTime / 3600) % 24;
    int minutes = static_cast<int>(elapsedTime / 60) % 60;
    int seconds = static_cast<int>(elapsedTime) % 60;

    std::ostringstream timeString;
    timeString << std::setw(2) << std::setfill('0') << hours << ":"
               << std::setw(2) << std::setfill('0') << minutes << ":"
               << std::setw(2) << std::setfill('0') << seconds;

    return timeString.str();
}

float TimeManager::getElapsedTime() const {
    return elapsedTime;
}

void TimeManager::incrementSocietyIteration() {
    societyIteration++;
}

void TimeManager::reset() {
    elapsedTime = 0.0f;
    currentDay = 1;
    societyIteration = 1;
}
