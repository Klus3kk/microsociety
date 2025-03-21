#ifndef TENSORFLOW_WRAPPER_HPP
#define TENSORFLOW_WRAPPER_HPP

#include <string>
#include <vector>
#include <memory>
#include "ActionType.hpp"
#include "State.hpp"

// Forward declare TF_Session to avoid including TensorFlow headers in header files
// This allows compilation even when TF is not available
struct TF_Graph;
struct TF_Session;
struct TF_Status;
struct TF_Tensor;

class TensorFlowWrapper {
private:
    // TensorFlow session and related objects
    TF_Graph* graph = nullptr;
    TF_Session* session = nullptr;
    TF_Status* status = nullptr;
    
    bool isInitialized = false;
    std::string modelPath;
    
    // Input and output tensor information
    std::string inputOpName;
    std::string outputOpName;
    
    // Helper methods
    void cleanupTensorFlow();
    TF_Tensor* createInputTensor(const std::vector<float>& stateVector) const;
    ActionType interpretOutput(TF_Tensor* outputTensor) const;
    
public:
    TensorFlowWrapper();
    ~TensorFlowWrapper();
    
    // Initialize TF model
    bool initialize(const std::string& modelPath);
    
    // Predict action using TF model
    ActionType predictAction(const State& state);
    
    // Convert state to vector for TF input
    std::vector<float> stateToVector(const State& state) const;
    
    // Check if TF is properly initialized
    bool isModelLoaded() const { return isInitialized; }
};

#endif 