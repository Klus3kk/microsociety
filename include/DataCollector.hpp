#ifndef DATA_COLLECTOR_HPP
#define DATA_COLLECTOR_HPP

#include "State.hpp"
#include "ActionType.hpp"
#include <vector>
#include <string>
#include <fstream>
#include <memory>
#include <mutex>
#include <nlohmann/json.hpp>

// Data structure for storing experience tuples
struct ExperienceData {
    State state;
    ActionType action;
    float reward;
    State nextState;
    bool done;
    std::string npcName;
    float timestamp;
    
    ExperienceData(const State& s, ActionType a, float r, const State& ns, 
                   bool d, const std::string& name, float time)
        : state(s), action(a), reward(r), nextState(ns), done(d), 
          npcName(name), timestamp(time) {}
    
    // Convert to JSON for export
    nlohmann::json toJson() const {
        return {
            {"state", {
                {"posX", state.posX},
                {"posY", state.posY},
                {"nearbyTrees", state.nearbyTrees},
                {"nearbyRocks", state.nearbyRocks},
                {"nearbyBushes", state.nearbyBushes},
                {"energyLevel", state.energyLevel},
                {"inventoryLevel", state.inventoryLevel}
            }},
            {"action", static_cast<int>(action)},
            {"reward", reward},
            {"nextState", {
                {"posX", nextState.posX},
                {"posY", nextState.posY},
                {"nearbyTrees", nextState.nearbyTrees},
                {"nearbyRocks", nextState.nearbyRocks},
                {"nearbyBushes", nextState.nearbyBushes},
                {"energyLevel", nextState.energyLevel},
                {"inventoryLevel", nextState.inventoryLevel}
            }},
            {"done", done},
            {"npcName", npcName},
            {"timestamp", timestamp}
        };
    }
};

class DataCollector {
private:
    std::vector<ExperienceData> experiences;
    std::string outputDirectory;
    std::string currentSessionFile;
    bool isCollecting = false;
    size_t maxExperiencesPerFile = 10000;
    size_t currentFileIndex = 0;
    mutable std::mutex dataMutex;
    
    // Statistics
    size_t totalExperiences = 0;
    size_t experiencesThisSession = 0;
    std::unordered_map<int, size_t> actionCounts;  // Count actions for balance analysis
    
    // Helper methods
    void createOutputDirectory();
    void saveCurrentBatch();
    std::string generateFilename();
    void updateStatistics(const ExperienceData& exp);
    
public:
    DataCollector(const std::string& outputDir = "training_data");
    ~DataCollector();
    
    void forceSaveCurrentBatch();

    std::vector<ExperienceData> getCurrentBatch() const;

    // Main interface
    void startCollection();
    void stopCollection();
    void recordExperience(const State& state, ActionType action, float reward, 
                         const State& nextState, bool done, const std::string& npcName);
    
    // Batch operations
    void saveDataToFile(const std::string& filename = "");
    void loadDataFromFile(const std::string& filename);
    void clearCurrentData();
    
    // Data export for Python
    void exportToCSV(const std::string& filename);
    void exportToJSON(const std::string& filename);
    void exportToNumpyFormat(const std::string& baseFilename);  // Creates .npz files
    
    // Statistics and monitoring
    size_t getTotalExperiences() const { return totalExperiences; }
    size_t getCurrentBatchSize() const { return experiences.size(); }
    bool isCollectingData() const { return isCollecting; }
    void printStatistics() const;
    void saveStatistics(const std::string& filename) const;
    
    // Configuration
    void setMaxExperiencesPerFile(size_t max) { maxExperiencesPerFile = max; }
    void setOutputDirectory(const std::string& dir);
    
    // Data quality analysis
    std::unordered_map<int, float> getActionDistribution() const;
    float getAverageReward() const;
    std::pair<float, float> getRewardRange() const;  // min, max
    void analyzeDataQuality() const;
};

// Singleton pattern for global access
DataCollector& getDataCollector();

#endif 

