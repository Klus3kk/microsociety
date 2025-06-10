#include "DataCollector.hpp"
#include "debug.hpp"
#include <filesystem>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>
#include <ctime>

DataCollector::DataCollector(const std::string& outputDir) 
    : outputDirectory(outputDir) {
    createOutputDirectory();
    currentSessionFile = generateFilename();
}

DataCollector::~DataCollector() {
    if (isCollecting) {
        stopCollection();
    }
}

void DataCollector::createOutputDirectory() {
    try {
        std::filesystem::create_directories(outputDirectory);
        std::filesystem::create_directories(outputDirectory + "/sessions");
        std::filesystem::create_directories(outputDirectory + "/exports");
        getDebugConsole().log("DataCollector", "Created output directories in: " + outputDirectory);
    } catch (const std::exception& e) {
        getDebugConsole().log("DataCollector", "Failed to create directories: " + std::string(e.what()), LogLevel::Error);
    }
}

void DataCollector::startCollection() {
    std::lock_guard<std::mutex> lock(dataMutex);
    isCollecting = true;
    experiencesThisSession = 0;
    currentSessionFile = generateFilename();
    getDebugConsole().log("DataCollector", "Started data collection session: " + currentSessionFile);
}

void DataCollector::stopCollection() {
    std::lock_guard<std::mutex> lock(dataMutex);
    if (isCollecting) {
        saveCurrentBatch();
        isCollecting = false;
        getDebugConsole().log("DataCollector", "Stopped data collection. Total experiences this session: " + 
                            std::to_string(experiencesThisSession));
    }
}

void DataCollector::recordExperience(const State& state, ActionType action, float reward, 
                                    const State& nextState, bool done, const std::string& npcName) {
    if (!isCollecting) {
        getDebugConsole().log("DataCollection", "WARNING: Not collecting, ignoring experience", LogLevel::Warning);
        return;
    }
    
    std::lock_guard<std::mutex> lock(dataMutex);
    
    float timestamp = static_cast<float>(std::time(nullptr));
    ExperienceData exp(state, action, reward, nextState, done, npcName, timestamp);
    experiences.push_back(exp);
    
    // Update statistics BEFORE incrementing total
    updateStatistics(exp);
    
    // Log detailed info for debugging
    getDebugConsole().log("DataCollection", 
        "RECORDED: " + npcName + 
        " | Action=" + std::to_string(static_cast<int>(action)) + 
        " | Reward=" + std::to_string(reward) + 
        " | State=(" + std::to_string(state.posX) + "," + std::to_string(state.posY) + ")" +
        " | Energy=" + std::to_string(state.energyLevel) +
        " | Inventory=" + std::to_string(state.inventoryLevel) +
        " | Batch Size=" + std::to_string(experiences.size()));
    
    experiencesThisSession++;
    
    // Auto-save when batch is full
    if (experiences.size() >= maxExperiencesPerFile) {
        getDebugConsole().log("DataCollection", "Batch full (" + std::to_string(experiences.size()) + 
                            "), auto-saving...");
        saveCurrentBatch();
    }
    
    // Log every 10 experiences for monitoring
    if (experiencesThisSession % 10 == 0) {
        getDebugConsole().log("DataCollection", "Session progress: " + 
                            std::to_string(experiencesThisSession) + " experiences, " +
                            "Current batch: " + std::to_string(experiences.size()));
    }
}

void DataCollector::forceSaveCurrentBatch() {
    std::lock_guard<std::mutex> lock(dataMutex);
    if (!experiences.empty()) {
        saveCurrentBatch();
    }
}

std::vector<ExperienceData> DataCollector::getCurrentBatch() const {
    std::lock_guard<std::mutex> lock(dataMutex);
    return experiences;
}


void DataCollector::saveCurrentBatch() {
    if (experiences.empty()) {
        getDebugConsole().log("DataCollection", "saveCurrentBatch called but experiences is empty");
        return;
    }
    
    size_t batchSize = experiences.size();
    
    std::string filename = outputDirectory + "/sessions/" + currentSessionFile + 
                          "_batch_" + std::to_string(currentFileIndex) + ".json";
    
    nlohmann::json jsonData;
    jsonData["metadata"] = {
        {"total_experiences", batchSize},
        {"session_file", currentSessionFile},
        {"batch_index", currentFileIndex},
        {"timestamp", std::time(nullptr)},
        {"cumulative_total", totalExperiences + batchSize}  // What total will be after this save
    };
    
    jsonData["experiences"] = nlohmann::json::array();
    for (const auto& exp : experiences) {
        jsonData["experiences"].push_back(exp.toJson());
    }
    
    std::ofstream file(filename);
    if (file.is_open()) {
        file << jsonData.dump(2);
        file.close();
        
        // NOW increment totalExperiences after successful save
        totalExperiences += batchSize;
        
        getDebugConsole().log("DataCollection", 
            "SAVED BATCH: " + std::to_string(batchSize) + " experiences to " + filename + 
            " | Total experiences now: " + std::to_string(totalExperiences));
        
        experiences.clear();
        currentFileIndex++;
    } else {
        getDebugConsole().log("DataCollection", "FAILED to save batch to: " + filename, LogLevel::Error);
    }
}

std::string DataCollector::generateFilename() {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::ostringstream oss;
    oss << "session_" << std::put_time(&tm, "%Y%m%d_%H%M%S");
    return oss.str();
}

void DataCollector::updateStatistics(const ExperienceData& exp) {
    experiencesThisSession++;
    actionCounts[static_cast<int>(exp.action)]++;
}

void DataCollector::exportToJSON(const std::string& filename) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::string fullPath = outputDirectory + "/exports/" + 
                          (filename.empty() ? "exported_data.json" : filename);
    
    nlohmann::json exportData;
    exportData["metadata"] = {
        {"total_experiences", experiences.size()},
        {"export_timestamp", std::time(nullptr)},
        {"action_distribution", getActionDistribution()},
        {"average_reward", getAverageReward()}
    };
    
    exportData["data"] = nlohmann::json::array();
    for (const auto& exp : experiences) {
        exportData["data"].push_back(exp.toJson());
    }
    
    std::ofstream file(fullPath);
    if (file.is_open()) {
        file << exportData.dump(2);
        getDebugConsole().log("DataCollector", "Exported " + std::to_string(experiences.size()) + 
                            " experiences to JSON: " + fullPath);
    }
}

void DataCollector::exportToCSV(const std::string& filename) {
    std::lock_guard<std::mutex> lock(dataMutex);
    
    std::string fullPath = outputDirectory + "/exports/" + 
                          (filename.empty() ? "training_data.csv" : filename);
    
    std::ofstream file(fullPath);
    if (!file.is_open()) {
        getDebugConsole().log("DataCollector", "Failed to create CSV file: " + fullPath, LogLevel::Error);
        return;
    }
    
    // CSV Header
    file << "state_posX,state_posY,state_nearbyTrees,state_nearbyRocks,state_nearbyBushes,"
         << "state_energyLevel,state_inventoryLevel,action,reward,"
         << "nextState_posX,nextState_posY,nextState_nearbyTrees,nextState_nearbyRocks,"
         << "nextState_nearbyBushes,nextState_energyLevel,nextState_inventoryLevel,"
         << "done,npcName,timestamp\n";
    
    size_t rowsWritten = 0;
    
    // FIRST: Export current batch (in-memory experiences)
    for (const auto& exp : experiences) {
        file << exp.state.posX << "," << exp.state.posY << ","
             << exp.state.nearbyTrees << "," << exp.state.nearbyRocks << ","
             << exp.state.nearbyBushes << "," << exp.state.energyLevel << ","
             << exp.state.inventoryLevel << "," << static_cast<int>(exp.action) << ","
             << exp.reward << "," << exp.nextState.posX << "," << exp.nextState.posY << ","
             << exp.nextState.nearbyTrees << "," << exp.nextState.nearbyRocks << ","
             << exp.nextState.nearbyBushes << "," << exp.nextState.energyLevel << ","
             << exp.nextState.inventoryLevel << "," << (exp.done ? 1 : 0) << ","
             << "\"" << exp.npcName << "\"," << exp.timestamp << "\n";
        rowsWritten++;
    }
    
    // SECOND: Read and append all saved batch files
    try {
        for (size_t i = 0; i < currentFileIndex; i++) {
            std::string batchFile = outputDirectory + "/sessions/" + currentSessionFile + 
                                  "_batch_" + std::to_string(i) + ".json";
            
            std::ifstream jsonFile(batchFile);
            if (jsonFile.is_open()) {
                nlohmann::json batchData;
                jsonFile >> batchData;
                
                if (batchData.contains("experiences")) {
                    for (const auto& expJson : batchData["experiences"]) {
                        // Extract data from JSON and write to CSV
                        file << expJson["state"]["posX"] << "," << expJson["state"]["posY"] << ","
                             << expJson["state"]["nearbyTrees"] << "," << expJson["state"]["nearbyRocks"] << ","
                             << expJson["state"]["nearbyBushes"] << "," << expJson["state"]["energyLevel"] << ","
                             << expJson["state"]["inventoryLevel"] << "," << expJson["action"] << ","
                             << expJson["reward"] << "," << expJson["nextState"]["posX"] << ","
                             << expJson["nextState"]["posY"] << "," << expJson["nextState"]["nearbyTrees"] << ","
                             << expJson["nextState"]["nearbyRocks"] << "," << expJson["nextState"]["nearbyBushes"] << ","
                             << expJson["nextState"]["energyLevel"] << "," << expJson["nextState"]["inventoryLevel"] << ","
                             << (expJson["done"] ? 1 : 0) << "," << "\"" << expJson["npcName"].get<std::string>() << "\","
                             << expJson["timestamp"] << "\n";
                        rowsWritten++;
                    }
                }
            }
        }
    } catch (const std::exception& e) {
        getDebugConsole().log("DataCollector", "Error reading batch files for CSV export: " + std::string(e.what()), LogLevel::Error);
    }
    
    file.close();
    
    getDebugConsole().log("DataCollector", 
        "CSV Export Complete: " + std::to_string(rowsWritten) + " total rows written to " + fullPath +
        " (Current batch: " + std::to_string(experiences.size()) + 
        ", Saved batches: " + std::to_string(currentFileIndex) + ")");
}

std::unordered_map<int, float> DataCollector::getActionDistribution() const {
    std::unordered_map<int, float> distribution;
    size_t total = std::accumulate(actionCounts.begin(), actionCounts.end(), 0,
                                  [](size_t sum, const auto& pair) { return sum + pair.second; });
    
    if (total == 0) return distribution;
    
    for (const auto& [action, count] : actionCounts) {
        distribution[action] = static_cast<float>(count) / total;
    }
    
    return distribution;
}

float DataCollector::getAverageReward() const {
    if (experiences.empty()) return 0.0f;
    
    float sum = std::accumulate(experiences.begin(), experiences.end(), 0.0f,
                               [](float sum, const ExperienceData& exp) { return sum + exp.reward; });
    return sum / experiences.size();
}

void DataCollector::printStatistics() const {
    getDebugConsole().log("DataCollector", "=== Data Collection Statistics ===");
    getDebugConsole().log("DataCollector", "Total experiences: " + std::to_string(totalExperiences));
    getDebugConsole().log("DataCollector", "Current batch size: " + std::to_string(experiences.size()));
    getDebugConsole().log("DataCollector", "Average reward: " + std::to_string(getAverageReward()));
    
    auto distribution = getActionDistribution();
    getDebugConsole().log("DataCollector", "Action distribution:");
    for (const auto& [action, percentage] : distribution) {
        getDebugConsole().log("DataCollector", "  Action " + std::to_string(action) + 
                            ": " + std::to_string(percentage * 100) + "%");
    }
}

void DataCollector::analyzeDataQuality() const {
    getDebugConsole().log("DataCollector", "=== Data Quality Analysis ===");
    
    // Check action balance
    auto distribution = getActionDistribution();
    float maxPercentage = 0.0f, minPercentage = 1.0f;
    for (const auto& [action, percentage] : distribution) {
        maxPercentage = std::max(maxPercentage, percentage);
        minPercentage = std::min(minPercentage, percentage);
    }
    
    float imbalanceRatio = maxPercentage / minPercentage;
    getDebugConsole().log("DataCollector", "Action imbalance ratio: " + std::to_string(imbalanceRatio));
    
    if (imbalanceRatio > 5.0f) {
        getDebugConsole().log("DataCollector", "WARNING: High action imbalance detected!", LogLevel::Warning);
    }
    
    // Check reward distribution
    auto [minReward, maxReward] = getRewardRange();
    getDebugConsole().log("DataCollector", "Reward range: [" + std::to_string(minReward) + 
                        ", " + std::to_string(maxReward) + "]");
}

std::pair<float, float> DataCollector::getRewardRange() const {
    if (experiences.empty()) return {0.0f, 0.0f};
    
    auto minMaxReward = std::minmax_element(experiences.begin(), experiences.end(),
        [](const ExperienceData& a, const ExperienceData& b) {
            return a.reward < b.reward;
        });
    
    return {minMaxReward.first->reward, minMaxReward.second->reward};
}

// Singleton instance
DataCollector& getDataCollector() {
    static DataCollector instance;
    return instance;
}