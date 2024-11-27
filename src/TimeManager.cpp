#include "TimeManager.hpp"
#include <iomanip>
#include <sstream>

TimeManager::TimeManager() 
    : elapsedTime(0.0f), currentDay(1), societyIteration(1) {}

void TimeManager::update(float deltaTime) {
    elapsedTime += deltaTime;

    // Increment day and iteration after 24 hours
    if (elapsedTime >= 86400.0f) {  // 24 hours in seconds
        elapsedTime -= 86400.0f;    // Reset elapsed time for the next day
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
    int hours = static_cast<int>(elapsedTime / 3600) % 24;
    int minutes = static_cast<int>(elapsedTime / 60) % 60;

    std::ostringstream timeString;
    timeString << std::setw(2) << std::setfill('0') << hours << ":"
               << std::setw(2) << std::setfill('0') << minutes;

    return timeString.str();
}

void TimeManager::reset() {
    elapsedTime = 0.0f;
    currentDay = 1;
    societyIteration = 1;
}
